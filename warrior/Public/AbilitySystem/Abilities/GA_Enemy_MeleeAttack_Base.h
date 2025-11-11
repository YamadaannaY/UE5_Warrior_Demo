//Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"
#include "GA_Enemy_MeleeAttack_Base.generated.h"

// GA_Enemy_MeleeAttack_Base.h
UCLASS()
class UGA_Enemy_MeleeAttack_Base : public UWarriorEnemyGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	// Montage相关
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* PlayMontage;

	// 伤害相关
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;
    
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat InDamageScalableFloat;

	// Gameplay Cue相关
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGameplayTag WarningGameplayCueTag;
    
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	FGameplayTag WeaponHitSoundGameplayCueTag;
    
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float UnblockableAttackWarningSpawnOffset = 0.0f;

private:
	void ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const;
	FGameplayCueParameters MakeBlockGamePlayCueParams() const;
    
	void WaitEventAndDealDamage();
	UFUNCTION()
	void HandleApplyDamage(FGameplayEventData InPayLoad);
    UFUNCTION()
	void PlayMontageAndDealFinished();
	UFUNCTION()
	void PlayMontageInternal();
	UFUNCTION()
	void OnMontageFinished();
};