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
    
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        K2_EndAbility();
        return;
    }
    
    UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,WarriorGamePlayTags::Shared_Event_MeleeHit);
    WaitEventTask->EventReceived.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::HandleApplyDamage);
    WaitEventTask->ReadyForActivation();
    
    // 播放Montage
    if (!PlayMontage)
    {
        K2_EndAbility();
        return;
    }

    // 检查不可阻挡状态
    bool bIsUnBlockable = UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetEnemyCharacterFromActorInfo(), WarriorGamePlayTags::Enemy_Status_UnBlockable);

    if (bIsUnBlockable)
    {
        // 播放不可阻挡警告特效
        ExecuteGameplayCueToOwnerWithParams(WarningGameplayCueTag);
        
        FTimerHandle WarningTimer;
        GetWorld()->GetTimerManager().SetTimer(WarningTimer, this, &UGA_Enemy_MeleeAttack_Base::PlayMontageInternal, 0.2f, false);
    }
    else
    {
        
        UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,("EnemyPlayMontage"),PlayMontage);
    
        if (PlayMontageTask)
        {
            PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::K2_EndAbility);
            PlayMontageTask->OnBlendOut.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::K2_EndAbility);
            PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
            PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::K2_EndAbility);
            PlayMontageTask->ReadyForActivation();
        }
    }
}

void UGA_Enemy_MeleeAttack_Base::ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC) return;
    
    FGameplayCueParameters CueParams;
    CueParams.RawMagnitude = UnblockableAttackWarningSpawnOffset;
    
    ASC->ExecuteGameplayCue(InGameplayTag, CueParams);
}

void UGA_Enemy_MeleeAttack_Base::HandleApplyDamage(FGameplayEventData InPayLoad)
{
    const AActor* LocalTargetActor = InPayLoad.Target.Get();
    if (!LocalTargetActor)
    {
        return;
    }

    // 应用伤害效果
    FGameplayEffectSpecHandle DamageSpecHandle = MakeEnemyDamageEffectSpecHandle(DamageGameplayEffectClass, InDamageScalableFloat);
    if (DamageSpecHandle.IsValid())
    {
        NativeApplyEffectSpecHandleToTarget(const_cast<AActor*>(LocalTargetActor), DamageSpecHandle);
    }

    // 播放音效Cue
    ExecuteGameplayCueToOwnerWithParams(WeaponHitSoundGameplayCueTag);
    
    // 给目标发送受击反应Event触发HitReactGA
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(const_cast<AActor*>(LocalTargetActor), WarriorGamePlayTags::Shared_Event_HitReact, InPayLoad);
}

void UGA_Enemy_MeleeAttack_Base::PlayMontageInternal()
{
    UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,("EnemyPlayMontage"),PlayMontage);
    
    if (PlayMontageTask)
    {
        PlayMontageTask->OnCompleted.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::K2_EndAbility);
        PlayMontageTask->OnBlendOut.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::K2_EndAbility);
        PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayMontageTask->OnCancelled.AddDynamic(this, &UGA_Enemy_MeleeAttack_Base::K2_EndAbility);
        PlayMontageTask->ReadyForActivation();
    }
}