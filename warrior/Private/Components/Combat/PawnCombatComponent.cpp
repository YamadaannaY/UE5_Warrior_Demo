// Yu


#include "Components/Combat/PawnCombatComponent.h"
#include "Items/Weapon/WarriorWeaponBase.h"
#include  "WarriorDebugHelper.h"
#include "Components/BoxComponent.h"

void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,
                                               AWarriorWeaponBase* InWeaponToRegister, const bool bRegisterAsEquipped)
{
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister),
		TEXT("A Tag named %s has already been added as carried weapon"),
		*InWeaponTagToRegister.ToString());
	check(InWeaponToRegister)
	
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister,InWeaponToRegister);
	
	//将自定义委托与函数绑定，会在监听动态多播委托的回调函数中判断此绑定情况（Execute if bound）。
	InWeaponToRegister->OnWeaponHitTarget.BindUObject(this,&ThisClass::OnHitTargetActor);
	InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this,&ThisClass::OnWeaponPulledFromTargetActor);
	
	//判断此装备是否为需要被Player装备的武器，是则为CurrentEquippedWeaponTag赋值
	if (bRegisterAsEquipped)
	{
		CurrentEquippedWeaponTag=InWeaponTagToRegister;
	}
}

AWarriorWeaponBase*  UPawnCombatComponent::GetCharacterCarriedWeaponByTag(const FGameplayTag InWeaponToGet) const
{
	if (CharacterCarriedWeaponMap.Contains(InWeaponToGet))
	{
		//使用二重指针包装WeaponBase，目的是不让此指针修改Weapon，但我们可以手动通过*FoundWeapon修改weapon
		if (AWarriorWeaponBase* const*FoundWeapon=CharacterCarriedWeaponMap.Find(InWeaponToGet))
		{
			return *FoundWeapon;
		}
	}
	return nullptr ;
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		return nullptr;
	}
	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	//确定伤害类型，如果是Weapon，设置碰撞状态
	if (ToggleDamageType==EToggleDamageType::CurrentEquippedWeapon)
	{
		ToggleCurrentEquippedWeaponCollision(bShouldEnable);
	}
	//碰撞伤害类型不是武器就是双手，为Boss的DamageType，此时对其进行Type切换
	else
	{
		ToggleBodyCollisionBoxCollision(bShouldEnable,ToggleDamageType);
	}
}

void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	//子类具体实现
}


void UPawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	//子类具体实现
}

void UPawnCombatComponent::ToggleCurrentEquippedWeaponCollision(bool bShouldEnable)
{
	const AWarriorWeaponBase* WeaponToToggle=GetCharacterCurrentEquippedWeapon();
	check(WeaponToToggle);
		
	//控制是否允许碰撞产生的bool变量
	if (bShouldEnable)
	{
		//QueryOnly可以被检测到，也就是触发重叠事件等，但是不会有实际物理反应，比如碰撞
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//在碰撞切换时清空
		OverLappedActors.Empty();
	}
}

void UPawnCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	//
}
