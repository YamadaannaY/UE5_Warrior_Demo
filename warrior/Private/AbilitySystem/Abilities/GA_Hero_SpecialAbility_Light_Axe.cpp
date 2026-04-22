// Yu

#include "AbilitySystem/Abilities/GA_Hero_SpecialAbility_Light_Axe.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorGamePlayTags.h"

void UGA_Hero_SpecialAbility_Light_Axe::HandleEventReceived(FGameplayEventData InPayLoad)
{
	const AActor* LocalTargetActor=InPayLoad.Target.Get();
	
	//确定Cue参数：绑定位置为SkeletalMeshComponent
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	
	USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();
	FGameplayCueParameters CueParams;
	CueParams.TargetAttachComponent=SkeletalMeshComponent;
	ASC->ExecuteGameplayCue(GameplayCueTag,CueParams);
	
	const float InWeaponBaseDamage=GetHeroCombatComponentFromActorInfo()->GetHeroCurrentEquippedWeaponDamageAtLevel(GetAbilityLevel());
	FGameplayEffectSpecHandle DamageSpecHandle=MakeHeroDamageEffectSpecHandle
	(DamageGameplayEffectClass,InWeaponBaseDamage,WarriorGamePlayTags::Player_SetByCaller_AttackType_Light,UseComboCount);

	if (!DamageSpecHandle.IsValid())
	{
		return ;
	}
	
	NativeApplyEffectSpecHandleToTarget(const_cast<AActor*>(LocalTargetActor),DamageSpecHandle);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(const_cast<AActor*>(LocalTargetActor),WarriorGamePlayTags::Shared_Event_HitReact,InPayLoad);
}