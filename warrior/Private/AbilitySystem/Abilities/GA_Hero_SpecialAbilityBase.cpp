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
	UAbilityTask_PlayMontageAndWait* Task_PlayMontageAndWait=UAbilityTask_PlayMontageAndWait::
	CreatePlayMontageAndWaitProxy
	(this,TEXT("PlayMontageAndWait"),
		SpecialWeaponAbilityMontage,
		1,
		NAME_None
		);
	Task_PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontagePlayingFinished);
	Task_PlayMontageAndWait->ReadyForActivation();
	
	UAbilityTask_WaitGameplayEvent* Task_WaitGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,                                          
		WaitEventTag,
		nullptr,                                       
		false,  // OnlyTriggerOnce
		true    // OnlyMatchExact
	);
	if (Task_WaitGameplayEvent)
	{
		Task_WaitGameplayEvent->EventReceived.AddDynamic(this, &ThisClass::HandleEventReceived);
		Task_WaitGameplayEvent->ReadyForActivation();
	}
}

void UGA_Hero_SpecialAbilityBase::OnMontagePlayingFinished()
{
	EndAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true,false);
}



void UGA_Hero_SpecialAbilityBase::HandleEventReceived(FGameplayEventData InPayLoad)
{
		
}

void UGA_Hero_SpecialAbilityBase::CallCooldown()
{
	//检查并确认Ability是否可以执行（即检查Cost和Cooldown），如果可以执行，则自动应用Cost和Cooldown
	CommitAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo());

	//手动调用Cooldown
	GetHeroCharacterFromActorInfo()->GetHeroUIComponent()->OnAbilityCooldownBegin.Broadcast
	(CooldownAbilityInputTag,GetCooldownTimeRemaining(),GetCooldownTimeRemaining());
}


