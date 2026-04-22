// Yu

#include "GA_Enemy_Melee_Base.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void UGA_Enemy_Melee_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	bool HasUnblockableTag = UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetAvatarActorFromActorInfo(),WarriorGamePlayTags::Enemy_Status_UnBlockable);
	
	if (HasUnblockableTag)
	{
		//先触发0.2秒的不可阻挡Cue
		FGameplayCueParameters Parameters;
		Parameters.RawMagnitude = UnblockableAttackWarningOffset;
		K2_ExecuteGameplayCueWithParams(WeaponHitSoundGameplayCueTag,Parameters);
		
		FTimerHandle DelayTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle,this,&ThisClass::DealMelee,0.2f,false);
	}
	else
	{
		DealMelee();
	}
}

void UGA_Enemy_Melee_Base::DealMelee()
{
	UAbilityTask_PlayMontageAndWait* PlayMeleeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,Melee_Montage);
	PlayMeleeMontageTask->OnBlendOut.AddDynamic(this,&ThisClass::K2_EndAbility);
	PlayMeleeMontageTask->OnCompleted.AddDynamic(this,&ThisClass::K2_EndAbility);
	PlayMeleeMontageTask->OnCancelled.AddDynamic(this,&ThisClass::K2_EndAbility);
	PlayMeleeMontageTask->OnInterrupted.AddDynamic(this,&ThisClass::K2_EndAbility);
	PlayMeleeMontageTask->ReadyForActivation();
	
	UAbilityTask_WaitGameplayEvent* WaitMeleeHitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,WarriorGamePlayTags::Shared_Event_MeleeHit);
	WaitMeleeHitEvent->EventReceived.AddDynamic(this,&ThisClass::HandleApplyDamage);
	WaitMeleeHitEvent->ReadyForActivation();
}

void UGA_Enemy_Melee_Base::HandleApplyDamage(FGameplayEventData Data)
{
	FActiveGameplayEffectHandle SpecHandle = NativeApplyEffectSpecHandleToTarget(const_cast<AActor*>(Data.Target.Get()),MakeEnemyDamageEffectSpecHandle(MeleeEffect,InDamageScalableFloat));
	if (SpecHandle.IsValid())
	{
		K2_ExecuteGameplayCue(WeaponHitSoundGameplayCueTag, FGameplayEffectContextHandle());
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(),WarriorGamePlayTags::Shared_Event_HitReact,Data);
	}
}