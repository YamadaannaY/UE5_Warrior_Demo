// Yu

#include "AI/BTTask_RotateToFaceTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

#include "BehaviorTree/BlackboardData.h"

UBTTask_RotateToFaceTarget::UBTTask_RotateToFaceTarget()
{
	NodeName=TEXT("Native Rotate to Face Target Actor");
	AnglePrecision=10.f;
	RotationInterSpeed=5.f;
	//允许该任务节点每帧调用 TickTask() 函数。
	bNotifyTick=true;
	//让节点在任务完成时调用OnTaskFinished()回调。
	bNotifyTaskFinished=true;
	//控制任务节点在行为树中是否为每个 AI 创建独立实例。
	//如果 bCreateNodeInstance = false，必须实现GetInstanceMemorySize()并通过NodeMemory存储每个AI的运行时状态，
	//否则多AI同时执行任务会互相干扰。此处使用自定义结构体作为Memory，所以需要false
	bCreateNodeInstance=false;
	//编辑器效果：当你在行为树任务蓝图/属性面板里选择黑板键时，黑板键列表只显示 AActor 类型或子类的对象，避免绑定错误类型。
	//运行时效果：任务在运行时通过 OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName) 获取对象时，可以保证类型安全。
	InTargetToFaceKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(ThisClass,InTargetToFaceKey),AActor::StaticClass());
	//每个Task都需要初始化
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

void UBTTask_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (const UBlackboardData* BBAsset=GetBlackboardAsset())
	{
		InTargetToFaceKey.ResolveSelectedKey(*BBAsset);
	}
}

uint16 UBTTask_RotateToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotatorToFaceTargetTaskMemory);
}

FString UBTTask_RotateToFaceTarget::GetStaticDescription() const
{
	const FString KeyDescription = InTargetToFaceKey.SelectedKeyName.ToString();
	return FString::Printf(TEXT("Smoothly rotates to face %s Key until the angle precision :%.2f is reached"),*KeyDescription,AnglePrecision);
}

EBTNodeResult::Type UBTTask_RotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UObject* ActorObject=OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName);
	//Player
	AActor* TargetActor=Cast<AActor>(ActorObject);
	//AIPawn
	APawn* OwningPawn=OwnerComp.GetAIOwner()->GetPawn();
	//实例化Memory结构体
	FRotatorToFaceTargetTaskMemory* Memory= CastInstanceNodeMemory<FRotatorToFaceTargetTaskMemory>(NodeMemory);
	check(Memory);
	Memory->OwingPawn=OwningPawn;
	Memory->TargetActor=TargetActor;

	if (!Memory->IsValid())
	{
		//行为树根据父节点逻辑（Selector/Sequence）决定下一步
		return EBTNodeResult::Failed;
	}
	if (HasReachingAnglePrecision(OwningPawn,TargetActor))
	{
		//直接执行下一个节点
		Memory->Reset();
		return EBTNodeResult::Succeeded;
	}
	//任务仍在进行中（异步任务/持续任务），Tick 或者 latent 完成后再返回 Succeeded/Failed
	return EBTNodeResult::InProgress;
}

void UBTTask_RotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//根据Tick特性时刻更新值
	FRotatorToFaceTargetTaskMemory* Memory= CastInstanceNodeMemory<FRotatorToFaceTargetTaskMemory>(NodeMemory);
	if (!Memory->IsValid())
	{
		FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
	}
	if (HasReachingAnglePrecision(Memory->OwingPawn.Get(),Memory->TargetActor.Get()))
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
	}
	else
	{
		const FRotator LookAtRot=UKismetMathLibrary::FindLookAtRotation(Memory->OwingPawn->GetActorLocation(),Memory->TargetActor->GetActorLocation());
		const FRotator TargetRot=FMath::RInterpTo(Memory->OwingPawn->GetActorRotation(),LookAtRot,DeltaSeconds,RotationInterSpeed);
		Memory->OwingPawn->SetActorRotation(TargetRot);
	}
}

bool UBTTask_RotateToFaceTarget::HasReachingAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const
{
	const float Distance = FVector::Dist2D(QueryPawn->GetActorLocation(), TargetActor->GetActorLocation());
	if (Distance <= 50.f) return true; // 距离太近直接结束.防止因为距离太近被认为是重叠导致OwnerToTargetNormalized几乎是0出现bug
	
	const FVector OwnerForward=QueryPawn->GetActorForwardVector();
	const FVector OwnerToTargetNormalized=(TargetActor->GetActorLocation()-QueryPawn->GetActorLocation()).GetSafeNormal2D();
	const float DotResult=FVector::DotProduct(OwnerForward.GetSafeNormal2D(),OwnerToTargetNormalized);
	const float AngleDiff=UKismetMathLibrary::DegAcos(DotResult);
	return AngleDiff<=AnglePrecision;
}
