// 被受击Tag触发的逻辑，播放受击动画

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbilit_HitReact.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroGameplayAbility_HitReact : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	void SelectHitReactMontageAndPlay( const FGameplayEventData& InPayLoad);

	//计算二者角度差值，根据值判断DirectionTag以触发对应的Montage
	UFUNCTION(BlueprintPure,Category="Warrior|Hit React")
	FGameplayTag ComputeHitReactDirectionTag(const AActor* InAttacker,AActor* InVictim,float& OutAngleDifference);
private:
	UPROPERTY(EditDefaultsOnly, Category="HitReactMontage")
	UAnimMontage* HitReactMontage_Front;
	
	UPROPERTY(EditDefaultsOnly, Category="HitReactMontage")
	UAnimMontage* HitReactMontage_Back;
	
	UPROPERTY(EditDefaultsOnly, Category="HitReactMontage")
	UAnimMontage* HitReactMontage_Left;
	
	UPROPERTY(EditDefaultsOnly, Category="HitReactMontage")
	UAnimMontage* HitReactMontage_Right;
	
	UPROPERTY(VisibleDefaultsOnly, Category="HitReactMontage")
	UAnimMontage* HitReactMontageToPlay;
};
