// Enemy的普通攻击GA

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"
#include "GA_Enemy_Melee_Base.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UGA_Enemy_Melee_Base : public UWarriorEnemyGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	UPROPERTY(EditDefaultsOnly , DisplayName="Anim")
	UAnimMontage* Melee_Montage;
	
	UPROPERTY(EditDefaultsOnly,DisplayName="Damage")
	FScalableFloat InDamageScalableFloat;
	
	UPROPERTY(EditDefaultsOnly,DisplayName="GameplayCue")
	FGameplayTag WeaponHitSoundGameplayCueTag;
	
	UPROPERTY(EditDefaultsOnly,DisplayName="Display")
	float UnblockableAttackWarningOffset;
	
	TSubclassOf<UGameplayEffect> MeleeEffect;
	
	//ApplyDamage，触发SoundCue并触发目标的HitReactGA
	UFUNCTION()
	void HandleApplyDamage(FGameplayEventData Data);
	
	//具体的攻击逻辑
	void DealMelee();
};
