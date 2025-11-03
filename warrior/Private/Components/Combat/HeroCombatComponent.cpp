// Yu


#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapon/WarriorHeroWeapon.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorGamePlayTags.h"

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTags(FGameplayTag InWeaponTag) const
{
	//向下转换，需要保证类型正确。
	return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedEquippedWeapon() const
{
	return Cast<AWarriorHeroWeapon>(GetCharacterCurrentEquippedWeapon());
}

float UHeroCombatComponent::GetHeroCurrentEquippedWeaponDamageAtLevel(const float InLevel) const
{
	//获得伤害的等级，作为伤害逻辑中的系数
	return GetHeroCarriedEquippedWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	//确保一次攻击只能伤害一次单个对象
	if (OverLappedActors.Contains(HitActor))
	{
		return ;
	}
	//确保不会多次添加
	OverLappedActors.AddUnique(HitActor);
	
	//Metadata for a tag-based Gameplay Event,作为Event数据，也就是节点返回的PayLoad
	FGameplayEventData Data;
	Data.Instigator=GetMyOwningPawn();
	Data.Target=HitActor;
	
	//蓝图WaitGameplayEvent中Tag作为监听条件，将这个Event发送给节点
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetMyOwningPawn(),WarriorGamePlayTags::Shared_Event_MeleeHit,Data);
	//FGameplayEventData()默认构造，因为这个Event的意义在于触发，作用于自己，不需要额外的Data
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetMyOwningPawn(),WarriorGamePlayTags::Player_Event_HitPause,FGameplayEventData());

}

void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	/*UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwingPawn<APawn>(),WarriorGamePlayTags::Player_Event_HitPause,FGameplayEventData());*/
}
