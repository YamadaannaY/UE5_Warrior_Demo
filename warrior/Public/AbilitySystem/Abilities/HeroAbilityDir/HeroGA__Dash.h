//实现一次以最后一次输入为朝向的Dash，如果没有输入触发则默认为向后
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
	FVector GetDashDirection();

	UPROPERTY(EditDefaultsOnly)
	float DashStrength;

	UPROPERTY(EditDefaultsOnly)
	float DashTime;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DashGameplayCueTag;

	//配置ApplyRootMotionTask实现一次冲刺
	void Dashing();
	
	//在Task完成后移除Cue，结束GA
	UFUNCTION()
	void FinishDashing();
};
