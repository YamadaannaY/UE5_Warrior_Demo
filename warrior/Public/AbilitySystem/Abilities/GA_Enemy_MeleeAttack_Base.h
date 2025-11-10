// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"
#include "GA_Enemy_MeleeAttack_Base.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UGA_Enemy_MeleeAttack_Base : public UWarriorEnemyGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void PlayMontageAndDealFinished();

	UFUNCTION()
	FGameplayCueParameters MakeBlockGamePlayCueParams() const;

	UFUNCTION()
	void ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void WaitEventAndDealDamage();

	UFUNCTION()
	void HandleApplyDamage(FGameplayEventData InPayLoad);

	UPROPERTY()
	float UnblockableAttackWarningSpawnOffset;

	UPROPERTY()
	FGameplayTag WarningGameplayCueTag;

	UPROPERTY()
	FGameplayTag WeaponHitSoundGameplayCueTag;

	UPROPERTY()
	UAnimMontage* PlayMontage;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;

	UPROPERTY()
	FScalableFloat InDamageScalableFloat;
};
