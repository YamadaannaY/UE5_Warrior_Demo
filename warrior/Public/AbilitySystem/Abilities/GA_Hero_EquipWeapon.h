// 将武器装备到手中

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

	//播放Montage并进行装备
	void PlayMontageAndWaitEventEquip();
	
	UFUNCTION()
	void AttachWeapon(FGameplayEventData InPayload);

	void HandleEquipWeapon(AWarriorHeroWeapon* InWeaponToEquip);

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
