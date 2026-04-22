//Dash：无移动下默认触发为向后，移动下向移动方向触发

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
	//获取Dash方向
	FVector GetDashDirection();

	UPROPERTY(EditDefaultsOnly)
	float DashStrength;

	UPROPERTY(EditDefaultsOnly)
	float DashTime;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DashGameplayCueTag;

	//ApplyRootMotionTask实现一次冲刺
	void Dashing();
	
	//在Task完成后移除Cue，结束GA
	UFUNCTION()
	void FinishDashing();
};
