// Yu


 #include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"


void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(
	UWarriorAbilitySystemComponent* InASCToGive,
	const int32 ApplyLevel)
{
	check(InASCToGive);
	//一旦被Give直接Activate的能力（eg:SpawnWeapon）
	GrantAbilities(ActivateOnGivenAbilities,InASCToGive,ApplyLevel);
	//响应式能力，依赖事件触发，即不会主动激活的能力（eg：HitPause，触发为Event，通过SendEvent触发）
	GrantAbilities(ReactiveAbilities,InASCToGive,ApplyLevel);

	if (! StartUpGameplayEffects.IsEmpty())
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
		{
			if (!EffectClass) continue;

			//CDO (Class Default Object):保存了这个GE在编辑器中配置的所有所有静态数据
			const UGameplayEffect* EffectCDO=EffectClass->GetDefaultObject<UGameplayEffect>();
			//基于CDO构造Spec,带上了动态数据
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
		//~将能力赋予角色流程
		
		//创建这个能力的spec实例
		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject=InASCToGive->GetAvatarActor();
		AbilitySpec.Level=ApplyLevel;
		//注册这Spec到ActivatableAbilitiesAbilities列表，通过SpecHandle允许ASC激活能力
		InASCToGive->GiveAbility(AbilitySpec);
	}
}
