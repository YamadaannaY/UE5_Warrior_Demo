// Yu

#pragma once

#include "CoreMinimal.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "HeroCombatComponent.generated.h"
class AWarriorHeroWeapon;
class AWarriorWeaponBase;
/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroCombatComponent : public UPawnCombatComponent
{
	GENERATED_BODY()

public:
	//通过Tag获取武器（利用Map）
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	AWarriorHeroWeapon* GetHeroCarriedWeaponByTags(FGameplayTag InWeaponTag) const;

	//获取当前装备的武器（通过GetHeroCarriedWeaponByTags（CurrentEquippedWeaponTag））
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	AWarriorHeroWeapon* GetHeroCarriedEquippedWeapon() const;

	//获取当前武器伤害等级（FScalableFloat WeaponBaseDamage，编辑器中绑定CT）
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	float GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel)const;

	//与自定义委托绑定的回调函数，用于BeginOverLap
	virtual void OnHitTargetActor(AActor*HitActor) override;

	//与自定义委托绑定的回调函数，用于EndOverLap
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor) override;

};
