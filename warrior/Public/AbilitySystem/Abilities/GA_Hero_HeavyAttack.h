// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "GA_Hero_HeavyAttack.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UGA_Hero_HeavyAttack : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
	struct FGameplayCueTag;
	/**
	 * 
	 */
protected:
	//能力激活时自动调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void PlayMontageAndDealFinished();

	//设置一个定时器，GA是PerActor,会保存当前段数，在定时范围内再次激活GA可以激活下一段，同时每一段GA都会清除当前Timer
	// 否则ResetAttackComboCount
	UFUNCTION()
	void OnMontagePlayingFinished();

	UFUNCTION()
	void ResetAttackComboCount();

	//最先调用，清除定时器
	UFUNCTION()
	void ClearTimer();

	//判断是否具有终结技Tag，判断当前段数，最后选择当前GA的Montage
	UFUNCTION()
	void FindIfSpecialTagAndSetMontage();

	//伤害GE赋予
	UFUNCTION()
	void ApplyDamage();

	//
	UFUNCTION()
	void HandleApplyDamage(FGameplayEventData InPayLoad);

	//只执行一次的GameplayCue
	UFUNCTION()
	void ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const;

	//CueParams：选择将Cue绑定到Mesh上
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
	TMap<int8,UAnimMontage*> HeavyAttackMontages;
	
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
	float ResetComboTime=0.3f;
};
