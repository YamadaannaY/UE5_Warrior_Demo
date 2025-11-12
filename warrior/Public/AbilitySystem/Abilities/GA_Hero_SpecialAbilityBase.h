// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "GA_Hero_SpecialAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UGA_Hero_SpecialAbilityBase : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	void PlayMontageAndDealFinished();

	UFUNCTION()
	void OnMontagePlayingFinished();

	UFUNCTION()
	virtual void HandleEventReceived(FGameplayEventData InPayLoad);
	
	void CallCooldown();

private:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* SpecialWeaponAbilityMontage;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag CooldownAbilityInputTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WaitEventTag;
	
};
