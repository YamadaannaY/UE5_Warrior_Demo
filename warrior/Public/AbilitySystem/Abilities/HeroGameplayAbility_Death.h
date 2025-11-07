// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_Death.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroGameplayAbility_Death : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:
	//能力激活时自动调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//能力结束时自动调用
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	

private:
	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	TArray<UAnimMontage*> MontageToPlay;

	UFUNCTION()
	void OnMontageFinished();
};
