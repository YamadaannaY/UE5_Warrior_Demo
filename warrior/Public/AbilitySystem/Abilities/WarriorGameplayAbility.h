// Yu

#pragma once

#include "CoreMinimal.h" 
#include "Abilities/GameplayAbility.h"
#include "WarriorTypes/WarriorEnumType.h"
#include "WarriorGameplayAbility.generated.h"

class UPawnCombatComponent;
class UWarriorAbilitySystemComponent;
/**
 * 
 */

UCLASS()
class WARRIOR_API UWarriorGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	//自定义GA触发方式，默认为Triggered
	UPROPERTY(EditDefaultsOnly,Category="Warrior|Ability")
	EWarriorAbilityActivationPolicy AbilityActivationPolicy=EWarriorAbilityActivationPolicy::OnTriggered;

	//通过GA获得角色的CombatComponent
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	//通过GA获得角色ASC
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo() const;
	
	//在所有GA被加入到ActivatableGA列表之后选择其中的OnGiven能力直接激活
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override ;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	//将GE的SpecHandle传递给目标Actor，即作为施法者对目标造成影响
	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor,const FGameplayEffectSpecHandle& InSpecHandle) const;

	//蓝图函数：将SpecHandle应用到TargetActor上，直接激活GE
	UFUNCTION(BlueprintCallable,Category="Warrior|Ability",meta=(DisplayName="Apply Gameplay Effect Spec Handle To Target Actor",ExpandEnumAsExecs="OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,const FGameplayEffectSpecHandle& InSpecHandle,EWarriorSuccessType& OutSuccessType) const ;

	//AOE攻击(在重击技能GA中调用)：对所有HitResult的对象应用伤害GE，并触发其受击GA
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	void ApplyGameplayEffectSpecHandleToHitResult(const FGameplayEffectSpecHandle& InSpecHandle,const TArray<FHitResult>& InHitResults);
};
