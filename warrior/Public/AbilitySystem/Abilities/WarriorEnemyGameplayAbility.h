// Yu

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
	//GA下获得EnemyCharacter
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	AWarriorEnemyCharacter* GetEnemyCharacterFromActorInfo();

	//GA下获得EnemyCombatComponent
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UEnemyCombatComponent*  GetEnemyCombatComponentFromActorInfo() const;

private:
	TWeakObjectPtr<AWarriorEnemyCharacter> CachedEnemyCharacter;
};
