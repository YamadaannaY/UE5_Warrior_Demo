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
	//弱指针赋值成功后，解引用弱指针，
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


FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass,
	float InWeaponBaseDamage,
	FGameplayTag InCurrentAttackTypeTag,
	int32 InUsedComboCount)
{
	check(EffectClass);
	
	//EffectContext 保存当前Effect的来源信息、发起者、附加对象等
	FGameplayEffectContextHandle ContextHandle=GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	//第一个参数是 InstigatorActor（能力的发起者），第二个是 EffectCauser（造成伤害的来源，如武器，也可以是第一个参数）
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(),GetAvatarActorFromActorInfo());
	
	//创建 EffectSpecHandle
	FGameplayEffectSpecHandle EffectSpecHandle=GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EffectClass,
		GetAbilityLevel(),
		ContextHandle
		);
	
	//SetByCaller可以在EffectClass中定义数值参数,然后在运行时动态赋值(蓝图中自定义)
	//通过EffectSpecHandle.Data->SetSetByCallerMagnitude存储Tag和对应的属性值，
	//在GEE中：TPair<FGameplayTag,float>& TagMagnitude:EffectSpec.SetByCallerTagMagnitudes调用了这个对应的pair
	EffectSpecHandle.Data->SetSetByCallerMagnitude(WarriorGamePlayTags::Shared_SetByCaller_BaseDamage,InWeaponBaseDamage);

	if (InCurrentAttackTypeTag.IsValid())
	{
		//InCurrentAttackTypeTag为攻击类型，在轻击GA中用light创建spec，重击GA中用heavy创建spec
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag,InUsedComboCount);
	}
	return EffectSpecHandle;
}
