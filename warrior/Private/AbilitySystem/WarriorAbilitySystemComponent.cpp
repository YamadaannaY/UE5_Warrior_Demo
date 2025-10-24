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
	//对所有被GiveAbility的Ability进行遍历（可以被激活但尚未激活的能力，即HeroStartUpAbilitySets中带有Tag的GA）
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (! AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)) continue;
		//对于切换类型的能力进行专门判断
		if (InInputTag.MatchesTag(WarriorGamePlayTags::InputTag_Toggleable) && AbilitySpec.IsActive())
		{
				//激活蓝图中的OnEndAbility
				CancelAbilityHandle(AbilitySpec.Handle);
			}
			//如果没有激活，切换激活
		else
			{
				//激活能力（依赖输入，具有Tag）
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
}


void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
    //此函数只对所有InputTag_MustBeHeld下的Tag有效，因为Held类型的GA不会手动结束能力以达成持续效果
	if (!InInputTag.IsValid() || !InInputTag.MatchesTag(WarriorGamePlayTags::InputTag_MustBeHeld))
	{
		return;
	}
	//如果InputTag_MustBeHeld对应的GASpec正在被激活且此回调被触发（Completed）（即结束触发），则CancelGA
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags() .HasTagExact(InInputTag) && AbilitySpec.IsActive())
		{
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities
(const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities,const TArray<FWarriorHeroSpecialAbilitySet>& InSpecialWeaponAbilities,
	int32 ApplyLevel,TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	if (InDefaultWeaponAbilities.IsEmpty())
	{
		return ;
	}
	
	for (const FWarriorHeroAbilitySet& AbilitySet:InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;
		//Spec创建固定三步流程：GA、Source、level
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject=GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		
		//使用WeaponAbilities时利用Tag检索
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
		//存储所有建立好的WeaponAbilitySpec的句柄,这是为了便于删除。
		OutGrantedAbilitySpecHandles.Add(GiveAbility(AbilitySpec));
	}

	for (const FWarriorHeroSpecialAbilitySet& AbilitySet:InSpecialWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;
		//Spec创建固定三步流程：GA、Source、level
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject=GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
		OutGrantedAbilitySpecHandles.Add(GiveAbility(AbilitySpec));
	}
}

void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	//InSpecHandlesToRemove即OutGrantedAbilitySpecHandles，在蓝图中链接
	if (InSpecHandlesToRemove.IsEmpty())
	{
		return;
	}
	for (const FGameplayAbilitySpecHandle& SpecHandle:InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{	//Removes the specified ability
			ClearAbility(SpecHandle);
		}
	}
	InSpecHandlesToRemove.Empty();
}

bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());
	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	//用于需要调用具有相同标签多个技能时，比如有两个具有Enemy.Melee Tag的GA，用这个函数可以调用所有具有MatchingTag的GASpec
	//GetSingleTagContainer()把单个标签打包成集合，GetActivatableGameplayAbilitySpecsByAllMatchingTags要求使用Container接口进行标签匹配
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(),FoundAbilitySpecs);
	if (!FoundAbilitySpecs.IsEmpty())
	{
		//所有满足条件的GA中按照下标随机选择一个。
		//Note:此处不完全随机，是有规律的随机数可能需要改进。
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
