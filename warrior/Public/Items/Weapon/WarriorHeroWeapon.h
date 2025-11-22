// Yu

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapon/WarriorWeaponBase.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "WarriorHeroWeapon.generated.h"

/**
 * 
 */

UCLASS()
class WARRIOR_API AWarriorHeroWeapon : public AWarriorWeaponBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WeaponData")
	FWarriorHeroWeaponData HeroWeaponData;

	//存储被GiveAbility()到ASC的Spec句柄到本地变量GrantedAbilitySpecHandles
	void AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle> InSpecHandles);

	//获得存储GrantedAbilitySpecHandles的数组
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;

private:
	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};
