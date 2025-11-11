// Yu
#include "AbilitySystem/Abilities/GA_Enemy_MeleeAttack_Base.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"


void UGA_Enemy_MeleeAttack_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, 
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    // 检查Ability是否可以执行
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy melee attack commit failed"));
        CancelAbility(Handle, ActorInfo, ActivationInfo, true);
        return;
    }
    
    // 启动伤害事件监听
    WaitEventAndDealDamage();
    
    // 播放Montage
    PlayMontageAndDealFinished();
}

void UGA_Enemy_MeleeAttack_Base::ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC) return;
    ASC->ExecuteGameplayCue(InGameplayTag, MakeBlockGamePlayCueParams());
}

void UGA_Enemy_MeleeAttack_Base::WaitEventAndDealDamage()
{
    UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
        this,                                          
        WarriorGamePlayTags::Shared_Event_MeleeHit,
        nullptr,                                       
        false,  // OnlyTriggerOnce
        true    // OnlyMatchExact
    );
    
    if (WaitEventTask)
    {
        WaitEventTask->EventReceived.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::HandleApplyDamage);
        WaitEventTask->ReadyForActivation();
        UE_LOG(LogTemp, Warning, TEXT("Enemy damage event listener started"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create enemy damage event task"));
    }
}

void UGA_Enemy_MeleeAttack_Base::HandleApplyDamage(FGameplayEventData InPayLoad)
{
    const AActor* LocalTargetActor = InPayLoad.Target.Get();
    if (!LocalTargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy attack: Target actor is null"));
        return;
    }

    // 应用伤害效果
    FGameplayEffectSpecHandle DamageSpecHandle = MakeEnemyDamageEffectSpecHandle(DamageGameplayEffectClass, InDamageScalableFloat);
    if (DamageSpecHandle.IsValid())
    {
        NativeApplyEffectSpecHandleToTarget(const_cast<AActor*>(LocalTargetActor), DamageSpecHandle);
        UE_LOG(LogTemp, Warning, TEXT("Enemy applied damage to: %s"), *LocalTargetActor->GetName());
    }

    // 播放音效
    ExecuteGameplayCueToOwnerWithParams(WeaponHitSoundGameplayCueTag);
    
    // 发送受击反应事件
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(const_cast<AActor*>(LocalTargetActor), WarriorGamePlayTags::Shared_Event_HitReact, InPayLoad);
}

void UGA_Enemy_MeleeAttack_Base::PlayMontageAndDealFinished()
{
    // 检查Montage资源
    if (!PlayMontage)
    {
        UE_LOG(LogTemp, Error, TEXT("Enemy attack: PlayMontage is null"));
        EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, true);
        return;
    }

    // 检查不可阻挡状态
    bool bIsUnBlockable = UWarriorFunctionLibrary::NativeDoesActorHaveTag(
        GetEnemyCharacterFromActorInfo(), 
        WarriorGamePlayTags::Enemy_Status_UnBlockable
    );

    if (bIsUnBlockable)
    {
        // 播放警告特效
        ExecuteGameplayCueToOwnerWithParams(WarningGameplayCueTag);
        UE_LOG(LogTemp, Warning, TEXT("Enemy unblockable attack warning"));
        
        // 延迟播放Montage
        FTimerHandle WarningTimer;
        GetWorld()->GetTimerManager().SetTimer(WarningTimer, this, 
            &UGA_Enemy_MeleeAttack_Base::PlayMontageInternal, 0.2f, false);
    }
    else
    {
        // 立即播放Montage
        PlayMontageInternal();
    }
}

void UGA_Enemy_MeleeAttack_Base::PlayMontageInternal()
{
    UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,                                
        TEXT("EnemyPlayMontage"),            
        PlayMontage,                            
        1.0f,                                   
        NAME_None,                              
        true,   // bStopWhenAbilityEnds
        1.0f    // AnimRootMotionTranslationScale
    );
    
    if (PlayMontageTask)
    {
        PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::OnMontageFinished);
        PlayMontageTask->OnBlendOut.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::OnMontageFinished);
        PlayMontageTask->OnInterrupted.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::OnMontageFinished);
        PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::OnMontageFinished);
        PlayMontageTask->ReadyForActivation();
        
        UE_LOG(LogTemp, Warning, TEXT("Enemy started playing montage: %s"), *PlayMontage->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create enemy montage task"));
        EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, true);
    }
}

void UGA_Enemy_MeleeAttack_Base::OnMontageFinished()
{
    UE_LOG(LogTemp, Warning, TEXT("Enemy montage finished, ending ability"));
    
    // Montage播放完成，正常结束Ability
    EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}

FGameplayCueParameters UGA_Enemy_MeleeAttack_Base::MakeBlockGamePlayCueParams() const 
{
    FGameplayCueParameters CueParams;
    CueParams.RawMagnitude = UnblockableAttackWarningSpawnOffset;
    return CueParams;
}