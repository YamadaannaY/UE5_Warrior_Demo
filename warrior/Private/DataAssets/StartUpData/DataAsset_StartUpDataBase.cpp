// Yu


 #include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(
	UWarriorAbilitySystemComponent* InASCToGive,
	const int32 ApplyLevel)
{
	check(InASCToGive);
	
	GrantAbilities(ActivateOnGivenAbilities,InASCToGive,ApplyLevel);
	GrantAbilities(ReactiveAbilities,InASCToGive,ApplyLevel);
	
	if (! StartUpGameplayEffects.IsEmpty())
	{
		//当Actor 被PossessBy时，把一组预设好的GE自动应用用来初始化属性（生命、攻击力等）或添加被动状态。
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
		{
			if (!EffectClass) continue;

			//不需要实例进行操作，只获取CDO进行读取。
			const UGameplayEffect* EffectCDO=EffectClass->GetDefaultObject<UGameplayEffect>();
			//基于CDO构造Spec，应用到自己身上----------底层：创建Spec，绑定传入Context(Target、Source(在ASC初始化中确定的两个参数)),应用到自己身上
			InASCToGive->ApplyGameplayEffectToSelf(EffectCDO,ApplyLevel,InASCToGive->MakeEffectContext());
		}
	}
}

void  UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>> &InAbilitiesToGive,UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel=1)
{
	if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}
	
	for (const TSubclassOf<UWarriorGameplayAbility>& Ability: InAbilitiesToGive)
	{
		if (!Ability)
		{
			continue;
		}
		
		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject=InASCToGive->GetAvatarActor();
		AbilitySpec.Level=ApplyLevel;
		//注册Spec到ActivatableAbilitiesAbilities列表，通过SpecHandle允许ASC激活能力
		InASCToGive->GiveAbility(AbilitySpec);
	}
}
