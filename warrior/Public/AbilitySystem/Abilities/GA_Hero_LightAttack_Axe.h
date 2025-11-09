// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GA_Hero_LightAttack.h"
#include "GA_Hero_LightAttack_Axe.generated.h"

class AWarriorProjectileBase;
/**
 * 
 */
UCLASS()
class WARRIOR_API UGA_Hero_LightAttack_Axe : public UGA_Hero_LightAttack
{
	GENERATED_BODY()

private:
	virtual void WhileRageActive() override;

	UFUNCTION()
	void SpawnProjectile(FGameplayEventData InPayLoad);
private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWarriorProjectileBase> HeroSlash;
};
