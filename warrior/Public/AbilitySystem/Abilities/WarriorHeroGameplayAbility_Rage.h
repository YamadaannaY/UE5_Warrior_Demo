// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "WarriorHeroGameplayAbility_Rage.generated.h"

class UAbilityAsync_WaitGameplayTagAdded;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorHeroGameplayAbility_Rage : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnTaskCompleted() ;

	void RageActivating();

	void AddGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag);
	
	UFUNCTION()
	void OnGameplayEventReceived(FGameplayEventData Payload);
	
	void OnAdded(FGameplayTag Tag, int32 NewCount);
	
	void RemoveRageCostEffect();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditDefaultsOnly, Category = "CueTag")
	FGameplayTag GameplayCueTag;

	UPROPERTY()
	AWarriorHeroCharacter* HeroCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	FActiveGameplayEffectHandle ApplyRageCostCostGEHandle;
	
	FDelegateHandle TagAddedDelegateHandle;
};
