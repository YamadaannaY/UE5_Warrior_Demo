// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "GA_Hero_LightAttack.generated.h"

struct FGameplayCueTag;
/**
 * 
 */
UCLASS()
class WARRIOR_API UGA_Hero_LightAttack : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
protected:
	//能力激活时自动调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void PlayMontageAndDealFinished();

	UFUNCTION()
	void OnMontagePlayingFinished();

	UFUNCTION()
	void ResetAttackComboCount();

	UFUNCTION()
	void ClearTimer();

	UFUNCTION()
	void FindIfSpecialTagAndSetMontage();

	UFUNCTION()
	void ApplyDamage();

	UFUNCTION()
	void HandleApplyDamage(FGameplayEventData InPayLoad);

	UFUNCTION()
	void ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const;

	UFUNCTION()
	FGameplayCueParameters MakeBlockGamePlayCueParams() const ;

	UFUNCTION()
	void HandleComboCount();

	UFUNCTION()
	void SpecialAttackWithRage();

	UFUNCTION()
	virtual void WhileRageActive();

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<int8,UAnimMontage*> LightAttackMontages;
	
	UPROPERTY()
	UAnimMontage* CurrentPlayingMontage;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag GameplayCueTag;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GainRageGameplayEffectClass;
	
	UPROPERTY()
	int32 CurrentLayerAttackComboCount=1;

	UPROPERTY()
	int32 UseComboCount;
	
	FTimerHandle TimerHandle;

	UPROPERTY(EditDefaultsOnly)
	float ResetComboTime=3.f;
	
};
