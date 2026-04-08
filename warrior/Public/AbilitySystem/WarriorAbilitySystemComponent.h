// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "WarriorAbilitySystemComponent.generated.h"

/**
 * 对许多GA可复用的函数一般写在ASCComponent中
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

	//Give所有EquipWeapon状态下能够实现的GA,并存储Spec数组中
	UFUNCTION(BlueprintCallable,Category="Warrior|Ability",meta=(ApplyLevel="1"))
	void GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities,const TArray<FWarriorHeroSpecialAbilitySet>& InSpecialWeaponAbilities,int32 ApplyLevel,TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles);

	//Clear所有EquipWeapon状态下被Give到ASC的能力，用于UnequipWeapon
	UFUNCTION(BlueprintCallable,Category="Warrior|Ability")
	void RemoveGrantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);

	//找到所有具有此Tag的能力，随机取出一个实现
	UFUNCTION(BlueprintCallable,Category="Warrior|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
};
