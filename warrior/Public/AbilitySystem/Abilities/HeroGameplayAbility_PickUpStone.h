// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_PickUpStone.generated.h"
class AWarriorStoneBase;
/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroGameplayAbility_PickUpStone : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
protected:
	//~Begin GameplayAbility Interface
	
	//能力激活时自动调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//能力结束时自动调用
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	//~End GameplayAbility Interface

	UFUNCTION(BlueprintCallable)
	void CollectStones();

	UFUNCTION(BlueprintCallable)
	void ConsumeStones();

private:
	UPROPERTY(EditDefaultsOnly)
	float BoxTraceDistance=50.f;

	UPROPERTY(EditDefaultsOnly)
	FVector TraceBoxSize=FVector(100.f);

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery> > StoneTraceChannel;

	UPROPERTY(EditDefaultsOnly)
	bool bDrawDebugShape=false;

	UPROPERTY()
	TArray<AWarriorStoneBase*> CollectedStones;
};
