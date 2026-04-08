// 实现一个翻滚GA，服务器计算方向和落点，利用MotionWrap改变动画

#include "AbilitySystem/Abilities/HeroAbilityDir/HeroGA_Roll.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

/*-------------------------------------------
            Ability Activate
--------------------------------------------*/

void UHeroGA_Roll::ActivateAbility(const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    //服务端计算滚动方向和落点
    if (HasAuthority(&ActivationInfo))
    {
        ComputeRotateDirection();
    }
    
    UAbilityTask_PlayMontageAndWait* Task =UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,RollMontage);
    Task->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
    Task->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
    Task->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
    Task->OnBlendOut.AddDynamic(this,&ThisClass::K2_EndAbility);
    Task->ReadyForActivation();
}

/*-------------------------------------------
        Compute Roll Direction (Server)
--------------------------------------------*/

void UHeroGA_Roll::ComputeRotateDirection()
{
    AWarriorHeroCharacter* Hero = GetHeroCharacterFromActorInfo();
    if (!Hero) return;

    // 输入方向
    FVector InputDir = Hero->GetLastMovementInputVector().GetSafeNormal();
   
    if (InputDir.IsNearlyZero())
    {
        InputDir = Hero->GetActorForwardVector();
    }

    /*------- 落点检测（下方地面） -------*/
    const FVector TraceStart = Hero->GetActorLocation() + InputDir * HeroRollingDistance;
    const FVector TraceEnd = TraceStart + FVector(0, 0, -500);

    FHitResult Hit;
    UKismetSystemLibrary::LineTraceSingleForObjects(
        this,
        TraceStart,
        TraceEnd,
        RollLineTraceObjectType,
        false,
        TArray<AActor*>(),
        EDrawDebugTrace::None,
        Hit,
        true
    );
    FVector TargetLocation = Hit.bBlockingHit ? Hit.ImpactPoint : FVector::ZeroVector;
    
    Hero->Multicast_SetRollData(  InputDir, TargetLocation);
}
