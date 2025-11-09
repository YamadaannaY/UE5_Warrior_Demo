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

	UFUNCTION()
	void RageActivating();

	UFUNCTION()
	void AddGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag);

	UFUNCTION()
	FGameplayCueParameters MakeBlockGamePlayCueParams();

	UFUNCTION()
	void OnGameplayEventReceived(FGameplayEventData Payload);

	UFUNCTION()
	void OnAdded(FGameplayTag Tag, int32 NewCount);

	UFUNCTION()
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

	UPROPERTY()
	FActiveGameplayEffectHandle ApplyRageCostCostGEHandle;

	/*UPROPERTY()
	UAbilityAsync_WaitGameplayTagAdded* Task_WaitGameplayTagAdded;*/
	
	FDelegateHandle TagAddedDelegateHandle;
};
