// Yu

//ActorInfo存储了character、controller、component等信息

#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "Conrtroller/WarriorHeroController.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include"WarriorGamePlayTags.h"
#include "Characters/WarriorHeroCharacter.h"


AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	if (!CachedWarriorHeroCharacter.IsValid())
	{
		
		CachedWarriorHeroCharacter=Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}
	//赋值成功后获取弱指针，
	return CachedWarriorHeroCharacter.IsValid() ? CachedWarriorHeroCharacter.Get(): nullptr;
}


AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	if (!CachedWarriorHeroController.IsValid())
	{	
		CachedWarriorHeroController=Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
	}
	
	return CachedWarriorHeroController.IsValid() ? CachedWarriorHeroController.Get(): nullptr;
	
}


UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return  GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

UHeroUIComponent* UWarriorHeroGameplayAbility::GetHeroUIComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroUIComponent();
}


FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass,
	float InWeaponBaseDamage,
	FGameplayTag InCurrentAttackTypeTag,
	int32 InUsedComboCount)
{
	check(EffectClass);
	
	//GEContextHandle固定流程
	FGameplayEffectContextHandle ContextHandle=GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	//第一个参数是 InstigatorActor（能力的发起者），第二个是 EffectCauser（造成伤害的来源，如武器，也可以是第一个参数）
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(),GetAvatarActorFromActorInfo());
	
	//创建 EffectSpecHandle
	FGameplayEffectSpecHandle EffectSpecHandle=GetWarriorAbilitySystemComponentFromActorInfo()
	->MakeOutgoingSpec
	(EffectClass,
		GetAbilityLevel(),
		ContextHandle
		);
	
	//SetSetByCallerMagnitude实现动态赋值，在蓝图中传入特定值。Tag作为键值进行检索
	//在GEE中调用这些被设置值
	EffectSpecHandle.Data->SetSetByCallerMagnitude(WarriorGamePlayTags::Shared_SetByCaller_BaseDamage,InWeaponBaseDamage);

	if (InCurrentAttackTypeTag.IsValid())
	{
		//InCurrentAttackTypeTag为攻击类型，在GE中判断到对应的攻击计算，乘积系数不同，并且将InUsedComboCount代入公式
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag,InUsedComboCount);
	}
	return EffectSpecHandle;   
}

bool UWarriorHeroGameplayAbility::GetAbilityRemainingCoolDownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime,
	float& RemainingCooldownTime)
{
	check(InCooldownTag.IsValid());
	
	// 1. 创建冷却效果查询
	FGameplayEffectQuery CooldownQuery=FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags
	(InCooldownTag.GetSingleTagContainer());
	// 2. 获取所有匹配的冷却效果的时间和持续时间
	TArray<TPair<float,float>> TimeRemainingAndDuration=GetAbilitySystemComponentFromActorInfo()
	->GetActiveEffectsTimeRemainingAndDuration(CooldownQuery);
	// 3. 调用获得的两个时间值作输出引脚传给cooldown委托
	if (!TimeRemainingAndDuration.IsEmpty())
	{
		RemainingCooldownTime=TimeRemainingAndDuration[0].Key;
		TotalCooldownTime=TimeRemainingAndDuration[0].Value;
	}
	return RemainingCooldownTime>0.f;
}
