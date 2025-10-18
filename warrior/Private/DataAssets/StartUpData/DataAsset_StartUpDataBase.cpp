// Yu


 #include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(
	UWarriorAbilitySystemComponent* InASCToGive,
	const int32 ApplyLevel)
{
	check(InASCToGive);
	//AbilityActivationPolicy中设置为OnGiven，在OnGiveAbility中检索到此策略后直接激活，即被Give直接Activate的能力（eg:SpawnWeapon）
	GrantAbilities(ActivateOnGivenAbilities,InASCToGive,ApplyLevel);
	//响应式能力，依赖AbilityTriggers中Tag对应Event触发，即被动激活的能力（eg：HitPause，触发为Event，通过SendEvent触发）
	GrantAbilities(ReactiveAbilities,InASCToGive,ApplyLevel);

	if (! StartUpGameplayEffects.IsEmpty())
	{
		//当角色（或其ASC）初始化时，把一组预设好的GE自动应用到自身用来初始化属性（生命、攻击力等）或添加被动状态（比如“永久增加10%移速”）。
		for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
		{
			if (!EffectClass) continue;

			//CDO (Class Default Object):保存了这个GE在编辑器中配置的静态数据，在不创建实例的情况下进行读取。
			const UGameplayEffect* EffectCDO=EffectClass->GetDefaultObject<UGameplayEffect>();
			//基于CDO构造Spec，应用到自己身上，底层：创建Spec，绑定传入Context(Target、Source),应用到自己身上
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
