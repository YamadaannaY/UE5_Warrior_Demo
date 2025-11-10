// Yu


#include "AbilitySystem/Abilities/GA_Enemy_MeleeAttack_Base.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/GameplayStatics.h"

void UGA_Enemy_MeleeAttack_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	PlayMontageAndDealFinished();
	OnMontageCompleted();
}

void UGA_Enemy_MeleeAttack_Base::ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	ASC->ExecuteGameplayCue(InGameplayTag, MakeBlockGamePlayCueParams());
}

void UGA_Enemy_MeleeAttack_Base::OnMontageCompleted()
{
	CancelAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetAvatarActorFromActorInfo(),true);
}

void UGA_Enemy_MeleeAttack_Base::WaitEventAndDealDamage()
{
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		  this,                                          
		  WarriorGamePlayTags::Shared_Event_MeleeHit,
		  nullptr,                                       
		  false,                                         
		  true                                          
	  );
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.AddUniqueDynamic(this,&ThisClass::HandleApplyDamage);
		WaitEventTask->ReadyForActivation();
	}
}

void UGA_Enemy_MeleeAttack_Base::HandleApplyDamage(FGameplayEventData InPayLoad)
{
	const AActor* LocalTargetActor=InPayLoad.Target.Get();
	
	FGameplayEffectSpecHandle DamageSpecHandle=MakeEnemyDamageEffectSpecHandle(DamageGameplayEffectClass,InDamageScalableFloat);

	NativeApplyEffectSpecHandleToTarget(const_cast<AActor*>(LocalTargetActor),DamageSpecHandle);

	ExecuteGameplayCueToOwnerWithParams(WeaponHitSoundGameplayCueTag);
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(const_cast<AActor*>(LocalTargetActor),WarriorGamePlayTags::Shared_Event_HitReact,InPayLoad);
}



FGameplayCueParameters UGA_Enemy_MeleeAttack_Base::MakeBlockGamePlayCueParams() const 
{
	//确定Cue参数：绑定位置为SkeletalMeshComponent
	USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();
	FGameplayCueParameters CueParams;
	CueParams.RawMagnitude=UnblockableAttackWarningSpawnOffset;
	return CueParams;
}

void UGA_Enemy_MeleeAttack_Base::PlayMontageAndDealFinished()
{
	bool bIsUnBlockable=UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetEnemyCharacterFromActorInfo(),WarriorGamePlayTags::Enemy_Status_UnBlockable);

	if (bIsUnBlockable)
	{
		ExecuteGameplayCueToOwnerWithParams(WarningGameplayCueTag);
		FLatentActionInfo LatentInfo;
		UKismetSystemLibrary::Delay(this,0.2,LatentInfo);
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
this,                                
	TEXT("PlayMontageAndWait"),            
	PlayMontage,                            
	1.0f,                                   
	NAME_None,                              
	true);
	PlayMontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontageCompleted);
	PlayMontageTask->OnBlendOut.AddUniqueDynamic(this,&ThisClass::OnMontageCompleted);
	PlayMontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnMontageCompleted);
	PlayMontageTask->OnCancelled.AddUniqueDynamic(this,&ThisClass::OnMontageCompleted);

	PlayMontageTask->ReadyForActivation();
}


