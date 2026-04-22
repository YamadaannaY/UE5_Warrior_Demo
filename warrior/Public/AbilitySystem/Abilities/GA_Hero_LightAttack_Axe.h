// Axe轻击GA

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
	
	//具体Rage逻辑
	virtual void WhileRageActive() override;

	//产生刃气
	UFUNCTION()
	void SpawnProjectile(FGameplayEventData InPayLoad);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWarriorProjectileBase> HeroSlash;
};
