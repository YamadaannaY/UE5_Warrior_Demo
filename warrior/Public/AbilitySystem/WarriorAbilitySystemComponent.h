// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "WarriorAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	//当绑定映射输入时，激活对应能力，作为CallBack函数被能力调用
	void OnAbilityInputPressed(const FGameplayTag& InInputTag);

	//适用于需要持续输入的IA结束时调用，取消其SpecHandle
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	//生成所有EquipWeapon状态下能够实现的能力
	UFUNCTION(BlueprintCallable,Category="Warrior|Ability",meta=(ApplyLevel="1"))
	void GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities,int32 ApplyLevel,TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);

	//删除所有EquipWeapon状态下能够实现的能力，用于UnequipWeapon情景
	UFUNCTION(BlueprintCallable,Category="Warrior|Ability")
	void RemoveGrantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

	UFUNCTION(BlueprintCallable,Category="Warrior|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
};
