// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GA_Hero_HeavyAttack.h"
#include "GA_Hero_HeavyAttack_Axe.generated.h"

class AWarriorProjectileBase;
/**
 * 
 */
UCLASS()
class WARRIOR_API UGA_Hero_HeavyAttack_Axe : public UGA_Hero_HeavyAttack
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
