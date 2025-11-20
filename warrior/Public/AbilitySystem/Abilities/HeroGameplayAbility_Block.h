// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_Block.generated.h"

class UAbilityTask_WaitGameplayEvent;
/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroGameplayAbility_Block : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void AddGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const ;

	UFUNCTION()
	void ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const;


	UFUNCTION()
	FGameplayCueParameters MakeBlockGamePlayCueParams() const ;

	UFUNCTION()
	void PlayMontageAndDealFinished();

	UFUNCTION()
	void OnMontageFinished();

	//设置一个Timer,周期一过就删除Tag
	UFUNCTION()
	void StartResetJumpToFinishTimer();

	
	void ResetJumpToFinishState();

	UFUNCTION()
	void DealSuccessfulBlock(FGameplayEventData InPayLoad);

	UFUNCTION()
	void OnDelayCompleted();
private:
	float HeroBlockActiveTime;
	
	UPROPERTY(EditDefaultsOnly,Category = "CueTag")
	FGameplayTag GameplayCueShieldTag;
	UPROPERTY(EditDefaultsOnly,Category = "CueTag")
	FGameplayTag GameplayCueSuccessfulBlockTag;
	UPROPERTY(EditDefaultsOnly,Category = "CueTag")
	FGameplayTag GameplayCuePerfectBlockTag;
	
	UPROPERTY(EditDefaultsOnly,Category = "Montage")
	UAnimMontage* HeroBlockMontage;

	UPROPERTY()
	FTimerHandle AbilityTimerHandle;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitEventTask=nullptr;

	UPROPERTY()
	bool bIsPerfectBlock=false;

	UPROPERTY(EditDefaultsOnly,Category = "Ability")
	float PerfectBlockJudgeTime=0.25f;
};
