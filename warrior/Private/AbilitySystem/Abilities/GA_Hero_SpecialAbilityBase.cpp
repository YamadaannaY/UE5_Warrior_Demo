// Yu

#include "AbilitySystem/Abilities/GA_Hero_SpecialAbilityBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Components/UI/HeroUIComponent.h"

void UGA_Hero_SpecialAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PlayMontageAndDealFinished();
	CallCooldown();
}

void UGA_Hero_SpecialAbilityBase::PlayMontageAndDealFinished()
{
	UAbilityTask_PlayMontageAndWait* Task_PlayMontageAndWait=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy
	(this,NAME_None,SpecialWeaponAbilityMontage);
	Task_PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this,&ThisClass::K2_EndAbility);
	Task_PlayMontageAndWait->ReadyForActivation();
	
	UAbilityTask_WaitGameplayEvent* Task_WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,WaitEventTag);
	if (Task_WaitGameplayEvent)
	{
		Task_WaitGameplayEvent->EventReceived.AddDynamic(this, &ThisClass::HandleEventReceived);
		Task_WaitGameplayEvent->ReadyForActivation();
	}
}

void UGA_Hero_SpecialAbilityBase::HandleEventReceived(FGameplayEventData InPayLoad)
{
	//override in child class 
}

void UGA_Hero_SpecialAbilityBase::CallCooldown()
{
	K2_CommitAbility();

	//调用UI的Cooldown
	GetHeroCharacterFromActorInfo()->GetHeroUIComponent()->OnAbilityCooldownBegin.Broadcast
	(CooldownAbilityInputTag,GetCooldownTimeRemaining(),GetCooldownTimeRemaining());
}


