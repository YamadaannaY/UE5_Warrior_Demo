//将武器装备到手中的GA

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "GA_Hero_EquipWeapon.generated.h"

class AWarriorHeroWeapon;
/**
 * 
 */
UCLASS()
class UGA_Hero_EquipWeapon : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	//切换绑定的Socket点
	UFUNCTION()
	void AttachWeapon(FGameplayEventData InPayload);

	//Equip之后的AnimLayer切换、从Combat组件取出Weapon并处理WeaponData
	void HandleEquipWeapon(AWarriorHeroWeapon* InWeaponToEquip);

	//更新UI显示，每次重新计算冷却时间以防止冷却中GA在重新Equip后图标显示可释放
	void UpdateUIAndCalRemainingTime(AWarriorHeroWeapon* InWeaponToEquip);

private:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* PlayMontage;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EventEquipWeaponTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag CurrentEquipWeaponTag;

	UPROPERTY(EditDefaultsOnly)
	FName SocketName;
};
