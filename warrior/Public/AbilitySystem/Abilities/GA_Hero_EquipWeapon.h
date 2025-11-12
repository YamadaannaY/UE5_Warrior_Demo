// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "GA_Hero_EquipWeapon.generated.h"

class AWarriorHeroWeapon;
/**
 * 
 */
UCLASS()
class WARRIOR_API UGA_Hero_EquipWeapon : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void PlayMontageAndWaitEventEquip();

	UFUNCTION()
	void PlayMontageCompleted();

	UFUNCTION()
	void AttachWeapon(FGameplayEventData InPayload);

	UFUNCTION()
	void HandleEquipWeapon(AWarriorHeroWeapon* InWeaponToEquip);

	UFUNCTION()
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
