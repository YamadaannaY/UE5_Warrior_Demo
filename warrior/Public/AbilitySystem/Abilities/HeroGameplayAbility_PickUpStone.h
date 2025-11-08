// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_ExecuteOnTick.h"
#include "HeroGameplayAbility_PickUpStone.generated.h"
class UAbilityTask_WaitGameplayEvent;
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

	//收集Hit范围内所有Stone类型的Actor
	UFUNCTION(BlueprintCallable)
	void CollectStones(float DeltaTime);

	//消耗所有Stone
	UFUNCTION(BlueprintCallable)
	void ConsumeStones(FGameplayEventData EventData);
	
private:
	UPROPERTY(EditDefaultsOnly)
	float BoxTraceDistance=50.f;

	UPROPERTY(EditDefaultsOnly)
	FVector TraceBoxSize=FVector(100.f);

	//Dynamic
	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<	EObjectTypeQuery> > StoneTraceChannel;

	UPROPERTY(EditDefaultsOnly)
	bool bDrawDebugShape=false;

	UPROPERTY()
	TArray<AWarriorStoneBase*> CollectedStones;

	UPROPERTY()
	UAbilityTask_ExecuteOnTick* ExecuteOnTick=nullptr;

	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitEventTask=nullptr;
};
