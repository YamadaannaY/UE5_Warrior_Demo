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
		//当进入Rage状态时，每次攻击额外的逻辑
		virtual void WhileRageActive() override;

		//生成刃气
		UFUNCTION()
		void SpawnProjectile(FGameplayEventData InPayLoad);
	
		UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UGameplayEffect> GameplayEffectClass;

		UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWarriorProjectileBase> HeroSlash;
};
