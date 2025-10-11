// Yu


#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "NativeGameplayTags.h"
#include "WarriorGamePlayTags.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (! InInputTag.IsValid())
	{
		return ;
	}
	//所有ActivatableAbilitySpec遍历，也就是HeroStartUpData在HeroCharacter中被Give后的能力
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//在HeroData的AbilitySpec创建过程中AddTag，此处进行检索
		if (! AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)) continue;

		//激活能力（依赖输入，具有Tag）
		TryActivateAbility(AbilitySpec.Handle);
	}
}


void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
    //此函数只对所有InputTag_MustBeHeld下的Tag有效
	if (!InInputTag.IsValid() || !InInputTag.MatchesTag(WarriorGamePlayTags::InputTag_MustBeHeld))
	{
		return;
	}
	//InputTag_MustBeHeld被激活时，如果Spec对应的能力正在被激活且此回调被触发（Completed），则尝试Cancel
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags() .HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
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
		//存储所有建立好的WeaponAbilitySpec的句柄。
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

bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());
	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	//用于需要调用具有相同标签多个技能时，比如有两个Enemy.Melee的GA，用这个函数可以调用所有具有MatchingTag的GA
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(),FoundAbilitySpecs);
	if (!FoundAbilitySpecs.IsEmpty())
	{
		//所有满足条件的GA中按照下标随机选择一个。
		//此处不完全随机，是有规律的随机数可能需要改进。
		const int32 RandomAbilityIndex=FMath::RandRange(0,FoundAbilitySpecs.Num()-1);
		FGameplayAbilitySpec* SpecToActivate=FoundAbilitySpecs[RandomAbilityIndex];

		check(SpecToActivate);
		if (!SpecToActivate->IsActive())
		{
			return  TryActivateAbility(SpecToActivate->Handle);
		}
	}
	return false;
}
