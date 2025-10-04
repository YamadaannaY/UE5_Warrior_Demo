// Yu

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include  "GameplayTagContainer.h"
#include "WarriorTypes/WarriorEnumType.h"
#include "PawnCombatComponent.generated.h"



class AWarriorWeaponBase;
/**
 * 
 */
UCLASS()
class WARRIOR_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()
public:
	//蓝图函数：注册Weapon,即向CharacterCarriedWeaponMap中添加映射
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,AWarriorWeaponBase* InWeaponToRegister,bool bRegisterAsEquipped=false);

	//通过Tag的方式获得CharacterCarriedWeaponMap中存储的武器
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponToGet) const;

	//设置蓝图读写，以在蓝图中可控此Tag，当装备Weapon时添加这个额外标签到weapon中，卸下时删除标签
	UPROPERTY(BlueprintReadWrite,Category="Warrior|Combat`")
	FGameplayTag CurrentEquippedWeaponTag;

	//获得当前装备的武器
	UFUNCTION(Blueprintable,Category="Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;

	//转换武器碰撞状态，设置一个Bool变量，true->QueryOnly false->NoCollision
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	void ToggleWeaponCollision(bool bShouldEnable,EToggleDamageType ToggleDamageType=EToggleDamageType::CurrentEquippedWeapon);

	//OnComponentBeginOverlap下自定义委托OnHitTarget的回调函数，通过ExecuteIfBound实现
	virtual void OnHitTargetActor(AActor*HitActor);
	//OnComponentEndOverlap下自定义委托OnWeaponPulledFromTarget的回调函数，通过ExecuteIfBound实现
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);
	
protected:
	//存储OverLap的对象，避免一次攻击中对单个物体多次触发伤害，会在EndOverlap中被清空重置
	UPROPERTY()
	TArray<AActor*>	OverLappedActors;
	
private:
	//map映射：key：tag value：武器
	//使用指针作为对象，可以通过改变指针更改map中的内容。
	UPROPERTY()
	TMap<FGameplayTag,AWarriorWeaponBase*> CharacterCarriedWeaponMap;
	
	
};
