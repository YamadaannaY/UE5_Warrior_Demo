// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGA__Dash.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroGA__Dash : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	void Dashing();
	FVector GetDashDistance();

	UFUNCTION()
	void FinishDashing();

	UPROPERTY(EditDefaultsOnly)
	float DashStrength;

	UPROPERTY(EditDefaultsOnly)
	float DashTime;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DashGameplayCueTag;
};
