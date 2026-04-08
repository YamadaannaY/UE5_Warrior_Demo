
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGA_Roll.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroGA_Roll : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	//计算Roll的朝向和最终落点
	void ComputeRotateDirection();
	
	UPROPERTY(EditDefaultsOnly)
	float HeroRollingDistance;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> RollLineTraceObjectType;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* RollMontage;
};
