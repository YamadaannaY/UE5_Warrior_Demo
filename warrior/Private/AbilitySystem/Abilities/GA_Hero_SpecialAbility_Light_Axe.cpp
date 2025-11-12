// Yu


#include "AbilitySystem/Abilities/GA_Hero_SpecialAbility_Light_Axe.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorGamePlayTags.h"

void UGA_Hero_SpecialAbility_Light_Axe::HandleEventReceived(FGameplayEventData InPayLoad)
{
	const AActor* LocalTargetActor=InPayLoad.Target.Get();
	ExecuteGameplayCueToOwnerWithParams(GameplayCueTag);
	
	float InWeaponBaseDamage=GetHeroCombatComponentFromActorInfo()->GetHeroCurrentEquippedWeaponDamageAtLevel(GetAbilityLevel());
	FGameplayEffectSpecHandle DamageSpecHandle=MakeHeroDamageEffectSpecHandle(DamageGameplayEffectClass,InWeaponBaseDamage,WarriorGamePlayTags::Player_SetByCaller_AttackType_Light,UseComboCount);

	NativeApplyEffectSpecHandleToTarget(const_cast<AActor*>(LocalTargetActor),DamageSpecHandle);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(const_cast<AActor*>(LocalTargetActor),WarriorGamePlayTags::Shared_Event_HitReact,InPayLoad);
}

void UGA_Hero_SpecialAbility_Light_Axe::ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	ASC->ExecuteGameplayCue(InGameplayTag, MakeBlockGamePlayCueParams());
}



FGameplayCueParameters UGA_Hero_SpecialAbility_Light_Axe::MakeBlockGamePlayCueParams() const 
{
	//确定Cue参数：绑定位置为SkeletalMeshComponent
	USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();
	FGameplayCueParameters CueParams;
	CueParams.TargetAttachComponent=SkeletalMeshComponent;
	return CueParams;
}