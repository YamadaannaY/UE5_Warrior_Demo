// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "GA_Hero_UnequipWeapon.generated.h"

class AWarriorHeroWeapon;
/**
 * 
 */
UCLASS()
class WARRIOR_API UGA_Hero_UnequipWeapon : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	void PlayMontageAndWaitEventUnEquip();

	UFUNCTION()
	void PlayMontageCompleted();

	void OnTimerFinished();

	//将武器绑定到Socket上
	UFUNCTION()
	void AttachWeapon(FGameplayEventData InPayload);

	//取消武器手持后处理的逻辑，主要为移除UI图标、IMC、WeaponAbilities...
	UFUNCTION()
	void HandleUnEquipWeapon(AWarriorHeroWeapon* InWeaponToUnEquip);

	UFUNCTION()
	void UpdateUI(AWarriorHeroWeapon* InWeaponToUnEquip);

private:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* PlayUnequipMontage;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag EventUnEquipWeaponTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag CarriedWeaponTagToUse;

	UPROPERTY(EditDefaultsOnly)
	FName SocketName;
	
};
