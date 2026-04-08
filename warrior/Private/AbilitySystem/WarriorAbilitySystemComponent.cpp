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
	//对所有被GiveAbility的Ability进行遍历（可以被激活但尚未使用的GA，即HeroStartUpAbilitySets中动态赋予了Tag的GA）
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (! AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)) continue;
		
		//Toggleable GA没有手动结束能力，需要再次触发以解除GA
		if (InInputTag.MatchesTag(WarriorGamePlayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
				CancelAbilityHandle(AbilitySpec.Handle);
		}
		else
		{
				TryActivateAbility(AbilitySpec.Handle);
		}
	}
}


void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
    //此函数只对所有InputTag_MustBeHeld下的GA有效，因为Held类型的GA在激活后没有手动调用EndAbility，需要松开触发以结束能力
	if (!InInputTag.IsValid() || !InInputTag.MatchesTag(WarriorGamePlayTags::InputTag_MustBeHeld))
	{
		return;
	}
	
	//MustBeHeld对应的GASpec正在被激活且此回调被触发（Completed）则CancelGA
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags() .HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities,const TArray<FWarriorHeroSpecialAbilitySet>& InSpecialWeaponAbilities,int32 ApplyLevel,TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	if (InDefaultWeaponAbilities.IsEmpty())
	{
		return ;
	}
	
	for (const FWarriorHeroAbilitySet& AbilitySet:InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;
		
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject=GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
		
		//存储所有建立好的WeaponAbilitySpec的句柄,这是为了在Unequip下便于删除。
		OutGrantedAbilitySpecHandles.Add(GiveAbility(AbilitySpec));
	}

	for (const FWarriorHeroSpecialAbilitySet& AbilitySet:InSpecialWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject=GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
		
		OutGrantedAbilitySpecHandles.Add(GiveAbility(AbilitySpec));
	}
}

void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
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
	
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(),FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty())
	{

		const int32 RandomAbilityIndex=FMath::RandRange(0,FoundAbilitySpecs.Num()-1);
		const FGameplayAbilitySpec* SpecToActivate=FoundAbilitySpecs[RandomAbilityIndex];
		
		check(SpecToActivate);
		
		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}
	return false;
}