// Yu


#include "Components/Combat/PawnCombatComponent.h"
#include "Items/Weapon/WarriorWeaponBase.h"
#include  "WarriorDebugHelper.h"
#include "Components/BoxComponent.h"

void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,
                                               AWarriorWeaponBase* InWeaponToRegister, const bool bRegisterAsEquipped)
{
	if (!InWeaponToRegister || !IsValid(InWeaponToRegister))
		return;

	//防止重复注册
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister))
	{
		Debug::Print(FString::Printf(TEXT("Weapon %s already registered"), *InWeaponTagToRegister.ToString()));
		return;
	}
	
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister,InWeaponToRegister);
	
	//绑定委托（只在服务器或需要处理逻辑的一端）
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	
	if (OwnerPawn && OwnerPawn->HasAuthority())
	{
		InWeaponToRegister->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
		InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);
	}
    
	//设置装备状态（只在服务器）
	if (bRegisterAsEquipped && OwnerPawn && OwnerPawn->HasAuthority())
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
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
	//子类实现
}
