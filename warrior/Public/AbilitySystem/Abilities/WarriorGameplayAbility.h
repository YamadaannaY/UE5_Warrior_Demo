// Yu

#pragma once

#include "CoreMinimal.h" 
#include "Abilities/GameplayAbility.h"
#include "WarriorTypes/WarriorEnumType.h"
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
	//Ability触发方式，默认为Triggered，需要input才可以触发的Ability。可以在defaults中修改。
	UPROPERTY(EditDefaultsOnly,Category="Warrior|Ability")
	EWarriorAbilityActivationPolicy AbilityActivationPolicy=EWarriorAbilityActivationPolicy::OnTriggered;

	//通过GA获得角色的CombatComponent
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	//通过GA获得角色ASC
	UFUNCTION(BlueprintPure,Category="Warrior|Ability")
	UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo() const;
	
	//~ Begin UGameplayAbility Interface

	//跟随GiveAbility触发，将Spec注册到ASC的ActivatableAbilities（相当于学会的技能而不是释放的技能）列表。
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override ;
	//在能力完成时自动触发,如果是OnGiven类型的GA->ClearAbility(Spec),确保这个能力只在被赋予时触发
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	//~ End UGameplayAbility Interface

	//将GE的SpecHandle传递给目标Actor，即作为施法者对目标造成影响（eg：造成伤害）
	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor,const FGameplayEffectSpecHandle& InSpecHandle) const;

	//蓝图函数：传递SpecHandle给TargetActor
	UFUNCTION(BlueprintCallable,Category="Warrior|Ability",meta=(DisplayName="Apply Gameplay Effect Spec Handle To Target Actor",ExpandEnumAsExecs="OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,const FGameplayEffectSpecHandle& InSpecHandle,EWarriorSuccessType& OutSuccessType) const ;
};
