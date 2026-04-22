//EnemyGA

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorEnemyGameplayAbility.generated.h"
class UEnemyCombatComponent;
class AWarriorEnemyCharacter;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorEnemyGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()
public:
	//获得当前GA的所有者Character类
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	AWarriorEnemyCharacter* GetEnemyCharacterFromActorInfo();
	
	//获得Character的Combat组件
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UEnemyCombatComponent*  GetEnemyCombatComponentFromActorInfo() const;

	//制作一个SpecHandle，用SetByCaller实现动态赋值
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass,const FScalableFloat& InDamageScalableFloat);

private:
	//GA中的Character类生命周期十分复杂，用弱指针持有
	TWeakObjectPtr<AWarriorEnemyCharacter> CachedEnemyCharacter;
};
