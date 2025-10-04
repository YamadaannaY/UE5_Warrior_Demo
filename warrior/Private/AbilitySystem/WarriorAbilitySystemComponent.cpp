// Yu


#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (! InInputTag.IsValid())
	{
		return ;
	}
	//所有ActivatableAbilitySpec遍历，也就是HeroStartUpData在HeroCharacter中被Give后的能力
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//在HeroData的AbilitySpec创建过程中动态添加了Tag，此处进行检索
		if (! AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)) continue;

		//激活能力（依赖输入，具有Tag）
		TryActivateAbility(AbilitySpec.Handle);
	}
}


void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel,TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	if (InDefaultWeaponAbilities.IsEmpty())
	{
		return ;
	}
	
	for (const FWarriorHeroAbilitySet& AbilitySet:InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;
		//设置Spec固定三步流程
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject=GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		//使用WeaponAbilities时利用Tag检索
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
		//存储所有建立好的WeaponAbilitySpec
		OutGrantedAbilitySpecHandles.Add(GiveAbility(AbilitySpec));
	}
}

void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty())
	{
		return;
	}
	for (const FGameplayAbilitySpecHandle& SpecHandle:InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}
	InSpecHandlesToRemove.Empty();
}
