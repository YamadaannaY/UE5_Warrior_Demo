//重击GA

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
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	//设置一个定时器，GA是PerActor,会保存当前段数，在定时范围内再次激活GA可以激活下一段，同时每一段GA都会清除当前Timer否则ResetAttackComboCount
	UFUNCTION()
	void OnMontagePlayingFinished();

	void ResetAttackComboCount();
	
	//判断是否具有终结技Tag，判断当前段数，最后选择当前GA的Montage
	void FindIfSpecialTagAndSetMontage();
	
	UFUNCTION()
	void HandleApplyDamage(FGameplayEventData InPayLoad);

	//只执行一次的GameplayCue
	UFUNCTION()
	void ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const;

	//CueParams：选择将Cue绑定到Mesh上
	UFUNCTION()
	FGameplayCueParameters MakeBlockGamePlayCueParams() const ;
	
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
	
	int32 CurrentLayerAttackComboCount=1;

	int32 UseComboCount;
	
	FTimerHandle TimerHandle;

	UPROPERTY(EditDefaultsOnly)
	float ResetComboTime=0.3f;
};
