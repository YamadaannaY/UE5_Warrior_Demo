// Yu


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility_Rage.h"

#include "Characters/WarriorHeroCharacter.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayTag.h"

void UWarriorHeroGameplayAbility_Rage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	RageActivating();
	
}
void UWarriorHeroGameplayAbility_Rage::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(GameplayCueTag);
	RemoveRageCostEffect();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UWarriorHeroGameplayAbility_Rage::OnTaskCompleted()
{
	UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(HeroCharacter,WarriorGamePlayTags::Player_Status_Rage_Activating);
}

void UWarriorHeroGameplayAbility_Rage::RageActivating()
{
	HeroCharacter=GetHeroCharacterFromActorInfo();
	if (HeroCharacter)
	{
		UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(HeroCharacter,WarriorGamePlayTags::Player_Status_Rage_Activating);
	}
	UAbilityTask_PlayMontageAndWait* Task_PlayMontageAndWait=UAbilityTask_PlayMontageAndWait::
	CreatePlayMontageAndWaitProxy
	(this,
		TEXT("PlayMontageAndWait"),
		MontageToPlay,
		1,
		NAME_None,
		true
		);
	Task_PlayMontageAndWait->ReadyForActivation();
	Task_PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnTaskCompleted);
	Task_PlayMontageAndWait->OnCancelled.AddUniqueDynamic(this,&ThisClass::OnTaskCompleted);
	Task_PlayMontageAndWait->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnTaskCompleted);
	Task_PlayMontageAndWait->OnBlendOut.AddUniqueDynamic(this,&ThisClass::OnTaskCompleted);

	UAbilityTask_WaitGameplayEvent* Task_WaitGameplayEvent=UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
	  this,                                          
	  WarriorGamePlayTags::Player_Event_ActivateRage,
	  nullptr,                                       
	  false,                                         
	  true                                          
  );
	if (Task_WaitGameplayEvent)
	{
		Task_WaitGameplayEvent->EventReceived.AddUniqueDynamic(this,&ThisClass::OnGameplayEventReceived);
		Task_WaitGameplayEvent->ReadyForActivation();
	}

	/* //这个函数无法调用，不知道原因
	 *Task_WaitGameplayTagAdded=UAbilityAsync_WaitGameplayTagAdded::WaitGameplayTagAddToActor(GetAvatarActorFromActorInfo(),WarriorGamePlayTags::Player_Status_Rage_None,false);
	if (Task_WaitGameplayTagAdded)
	{
		Task_WaitGameplayTagAdded->Added.AddUniqueDynamic(this, &ThisClass::OnAdded);
	}*/
	
	UGameplayEffect* GameplayEffect = GameplayEffectClass.GetDefaultObject();
	ApplyRageCostCostGEHandle=ApplyGameplayEffectToOwner(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),GameplayEffect,GetAbilityLevel(),1);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		TagAddedDelegateHandle = ASC->RegisterGameplayTagEvent(
			WarriorGamePlayTags::Player_Status_Rage_None,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(this, &UWarriorHeroGameplayAbility_Rage::OnAdded); 
	}
}

FGameplayCueParameters UWarriorHeroGameplayAbility_Rage::MakeBlockGamePlayCueParams()
{
	//确定Cue参数：绑定位置为SkeletalMeshComponent
	USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();
	FGameplayCueParameters CueParams;
	CueParams.TargetAttachComponent=SkeletalMeshComponent;
	return CueParams;
}

void UWarriorHeroGameplayAbility_Rage::OnGameplayEventReceived(FGameplayEventData Payload)
{
	FGameplayTag CueTag = GameplayCueTag;
	AddGameplayCueToOwnerWithParams(CueTag);
}

void UWarriorHeroGameplayAbility_Rage::OnAdded(FGameplayTag Tag, int32 NewCount)
{
	CancelAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true);
}

void UWarriorHeroGameplayAbility_Rage::RemoveRageCostEffect()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !ApplyRageCostCostGEHandle.IsValid())
	{
		return;
	}
	ASC->RemoveActiveGameplayEffect(ApplyRageCostCostGEHandle);
}


void UWarriorHeroGameplayAbility_Rage::AddGameplayCueToOwnerWithParams(FGameplayTag InGameplayTag)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	ASC->AddGameplayCue(InGameplayTag, MakeBlockGamePlayCueParams());
}
