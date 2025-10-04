// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorHeroGameplayAbility.generated.h"
class UHeroCombatComponent;
class AWarriorHeroCharacter;
class AWarriorHeroController;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorHeroGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()
public:
	//GA下获得HeroCharacter
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	AWarriorHeroCharacter* GetHeroCharacterFromActorInfo();

	//GA下获得HeroController
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	AWarriorHeroController* GetHeroControllerFromActorInfo();

	//GA下获得HeroCombatComponent
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

	//这个函数的作用是 生成一个针对武器攻击的 GameplayEffectSpecHandle,用于应用GameplayEffect到目标,实现伤害或者其他效果。
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	FGameplayEffectSpecHandle MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass,float InWeaponBaseDamage,FGameplayTag InCurrentAttackTypeTag,int32 InUsedComboCount);
	
private:
	//缓存HeroCharacter
	TWeakObjectPtr<AWarriorHeroCharacter> CachedWarriorHeroCharacter;
	//缓存HeroController
	TWeakObjectPtr<AWarriorHeroController> CachedWarriorHeroController;
	
};
