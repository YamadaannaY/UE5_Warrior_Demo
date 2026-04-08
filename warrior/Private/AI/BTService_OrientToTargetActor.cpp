// Yu

#include "AI/BTService_OrientToTargetActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Conrtroller/WarriorAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardData.h"

UBTService_OrientToTargetActor::UBTService_OrientToTargetActor()
{	
	NodeName = TEXT("Native Orient Rotation To Target Actor ");
	
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	
	RotationInterSpeed=5.f; 
	Interval=0.f;			//检查一次黑板键值是否满足条件的频率
	RandomDeviation=0.5f;  //在interval之上加了随机值，使得各个AI判断条件具有浮动偏差，更加自然。
	
	InTargetActorKey.AddObjectFilter(this,GET_MEMBER_NAME_CHECKED(ThisClass,InTargetActorKey),AActor::StaticClass());
}

void UBTService_OrientToTargetActor::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (const UBlackboardData* BBAsset=GetBlackboardAsset())
	{
		//ResolveSelectedKey会查找UBlackboardData，把Key字面量解析成黑板Key的具体ID和类型。
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}


FString UBTService_OrientToTargetActor::GetStaticDescription() const
{
	const FString KeyDescription=InTargetActorKey.SelectedKeyName.ToString();
	return 	FString::Printf(TEXT("Orient rotation to %s key %s"),*KeyDescription,*GetStaticServiceDescription());
}


void UBTService_OrientToTargetActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	//返回行为树绑定的BB中对应FName的值。
	UObject* ActorObject=OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);

	//获取TargetActor
	const AActor* TargetActor=Cast<AActor>(ActorObject);

	//通过BT获得绑定的AIPawn
	APawn* OwningPawn=OwnerComp.GetAIOwner()->GetPawn();

	if (OwningPawn && TargetActor)
	{	
		//指向TargetActor
		const FRotator LookAtRot=UKismetMathLibrary::FindLookAtRotation(OwningPawn->GetActorLocation(),TargetActor->GetActorLocation());
		
		//插值优化实际表现
		const FRotator TargetRot=FMath::RInterpTo(OwningPawn->GetActorRotation(),LookAtRot,DeltaSeconds,RotationInterSpeed);
		OwningPawn->SetActorRotation(TargetRot);
	}
	
}
