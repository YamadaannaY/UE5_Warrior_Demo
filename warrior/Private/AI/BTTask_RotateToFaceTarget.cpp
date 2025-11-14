#include "AI/BTTask_RotateToFaceTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_RotateToFaceTarget::UBTTask_RotateToFaceTarget()
{
    NodeName = TEXT("Native Rotate to Face Target Actor");
    
    AnglePrecision = 5.f;
    RotationInterSpeed = 5.f;
    MinimumSafeDistance = 200.0f;
    bUseYawOnly = true;
    MaxRotationSpeed = 360.0f;
    AdaptiveSpeedMultiplier = 2.0f;
    
    bNotifyTick = true;
    bNotifyTaskFinished = true;
    bCreateNodeInstance = false;
    
    InTargetToFaceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetToFaceKey), AActor::StaticClass());
    
    INIT_TASK_NODE_NOTIFY_FLAGS();
}

void UBTTask_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);
    if (const UBlackboardData* BBAsset = GetBlackboardAsset())
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
    return FString::Printf(TEXT("Smoothly rotates to face %s\nPrecision: %.1f° | Safe Distance: %.0f"), 
        *KeyDescription, AnglePrecision, MinimumSafeDistance);
}

EBTNodeResult::Type UBTTask_RotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName);
    AActor* TargetActor = Cast<AActor>(ActorObject);
    APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
    
    FRotatorToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotatorToFaceTargetTaskMemory>(NodeMemory);
    check(Memory);
    
    Memory->OwingPawn = OwningPawn;
    Memory->TargetActor = TargetActor;
    Memory->LastSafeDirection = OwningPawn ? OwningPawn->GetActorForwardVector() : FVector::ForwardVector;

    if (!Memory->IsValid())
    {
        return EBTNodeResult::Failed;
    }
    
    if (HasReachingAnglePrecision(OwningPawn, TargetActor))
    {
        Memory->Reset();
        return EBTNodeResult::Succeeded;
    }
    
    return EBTNodeResult::InProgress;
}

void UBTTask_RotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    FRotatorToFaceTargetTaskMemory* Memory = CastInstanceNodeMemory<FRotatorToFaceTargetTaskMemory>(NodeMemory);

    if (!Memory->IsValid())
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }
    
    if (HasReachingAnglePrecision(Memory->OwingPawn.Get(), Memory->TargetActor.Get()))
    {
        Memory->Reset();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }
    
    // 使用安全的旋转计算
    PerformSafeRotation(Memory, DeltaSeconds);
}

void UBTTask_RotateToFaceTarget::PerformSafeRotation(FRotatorToFaceTargetTaskMemory* Memory, float DeltaSeconds)
{
    APawn* Pawn = Memory->OwingPawn.Get();
    AActor* Target = Memory->TargetActor.Get();
    
    if (!Pawn || !Target) return;

    FVector PawnLocation = Pawn->GetActorLocation();
    FVector TargetLocation = Target->GetActorLocation();
    
    // 计算到目标的向量
    FVector ToTarget = TargetLocation - PawnLocation;
    float Distance = ToTarget.Size2D(); // 使用2D距离
    
    // 关键修复：近距离稳定处理
    FVector SafeDirection;
    
    if (Distance < MinimumSafeDistance)
    {
        // 方法1：使用角色当前位置+安全距离的虚拟目标
        FVector DirectionToTarget = ToTarget.GetSafeNormal2D();
        if (DirectionToTarget.IsNearlyZero())
        {
            // 如果方向几乎为零，使用上一帧的安全方向
            SafeDirection = Memory->LastSafeDirection;
        }
        else
        {
            // 创建虚拟目标点
            FVector VirtualTarget = PawnLocation + DirectionToTarget * MinimumSafeDistance;
            SafeDirection = (VirtualTarget - PawnLocation).GetSafeNormal2D();
        }
    }
    else
    {
        // 正常距离，使用实际方向
        SafeDirection = ToTarget.GetSafeNormal2D();
    }
    
    // 保存安全方向供下一帧使用
    if (!SafeDirection.IsNearlyZero())
    {
        Memory->LastSafeDirection = SafeDirection;
    }
    
    // 计算目标旋转
    FRotator LookAtRot = SafeDirection.Rotation();
    FRotator CurrentRot = Pawn->GetActorRotation();
    
    // 应用轴向限制
    if (bUseYawOnly)
    {
        LookAtRot.Pitch = CurrentRot.Pitch;
        LookAtRot.Roll = CurrentRot.Roll;
    }
    
    // 自适应旋转速度
    float AdaptiveSpeed = CalculateAdaptiveRotationSpeed(CurrentRot, LookAtRot, Distance);
    
    // 使用更稳定的插值方法
    FRotator TargetRot = CalculateStableRotation(CurrentRot, LookAtRot, DeltaSeconds, AdaptiveSpeed);
    
    // 应用旋转
    Pawn->SetActorRotation(TargetRot);
}

float UBTTask_RotateToFaceTarget::CalculateAdaptiveRotationSpeed(const FRotator& CurrentRot, const FRotator& TargetRot, float Distance) const
{
    // 计算角度差
    FRotator DeltaRot = (TargetRot - CurrentRot).GetNormalized();
    float AngleDiff = FMath::Abs(DeltaRot.Yaw);
    
    float BaseSpeed = RotationInterSpeed;
    
    // 基于角度差的自适应速度
    if (AngleDiff > 120.0f)
    {
        // 大角度时快速旋转
        BaseSpeed *= AdaptiveSpeedMultiplier;
    }
    else if (AngleDiff < 20.0f)
    {
        // 小角度时精确旋转
        BaseSpeed *= 0.3f;
    }
    
    // 基于距离的调整：近距离时降低速度避免抖动
    if (Distance < MinimumSafeDistance * 2.0f)
    {
        float DistanceFactor = FMath::Clamp(Distance / MinimumSafeDistance, 0.3f, 1.0f);
        BaseSpeed *= DistanceFactor;
    }
    
    return BaseSpeed;
}

FRotator UBTTask_RotateToFaceTarget::CalculateStableRotation(const FRotator& CurrentRot, const FRotator& TargetRot, float DeltaSeconds, float Speed) const
{
    // 使用更稳定的旋转插值
    FRotator ResultRot;
    
    // 分别插值每个轴，避免万向锁问题
    ResultRot.Yaw = FMath::FInterpTo(CurrentRot.Yaw, TargetRot.Yaw, DeltaSeconds, Speed);
    
    if (bUseYawOnly)
    {
        ResultRot.Pitch = CurrentRot.Pitch;
        ResultRot.Roll = CurrentRot.Roll;
    }
    else
    {
        ResultRot.Pitch = FMath::FInterpTo(CurrentRot.Pitch, TargetRot.Pitch, DeltaSeconds, Speed);
        ResultRot.Roll = FMath::FInterpTo(CurrentRot.Roll, TargetRot.Roll, DeltaSeconds, Speed);
    }
    
    return ResultRot;
}

bool UBTTask_RotateToFaceTarget::HasReachingAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const
{
    if (!QueryPawn || !TargetActor) 
        return false;
        
    const float Distance = FVector::Dist2D(QueryPawn->GetActorLocation(), TargetActor->GetActorLocation());
    if (Distance <= 50.f) 
        return true; // 距离太近直接结束
    
    const FVector OwnerForward = QueryPawn->GetActorForwardVector().GetSafeNormal2D();
    const FVector OwnerToTargetNormalized = (TargetActor->GetActorLocation() - QueryPawn->GetActorLocation()).GetSafeNormal2D();
    
    // 如果方向几乎为零，认为已经面向目标
    if (OwnerToTargetNormalized.IsNearlyZero())
        return true;
    
    const float DotResult = FVector::DotProduct(OwnerForward, OwnerToTargetNormalized);
    const float AngleDiff = UKismetMathLibrary::DegAcos(DotResult);
    
    return AngleDiff <= AnglePrecision;
}