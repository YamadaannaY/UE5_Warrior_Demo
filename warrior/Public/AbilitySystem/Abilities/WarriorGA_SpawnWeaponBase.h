// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorGA_SpawnWeaponBase.generated.h"

class AWarriorWeaponBase;

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorGA_SpawnWeaponBase : public UWarriorGameplayAbility
{
	GENERATED_BODY()

protected:
	//能力激活时自动调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	TSubclassOf<AWarriorWeaponBase> WeaponClassToSpawn;

	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	FName SocketNameToAttachTo;

	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	bool RegisterAsEquipped;

	UPROPERTY(EditDefaultsOnly, Category="Abilities")
	FGameplayTag WeaponTagToRegister;
	
};
