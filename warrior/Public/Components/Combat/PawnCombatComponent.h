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
	//向CharacterCarriedWeaponMap中添加映射
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,AWarriorWeaponBase* InWeaponToRegister,bool bRegisterAsEquipped=false);

	//Tag作为键值寻找CharacterCarriedWeaponMap中存储的武器
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponToGet) const;

	//在蓝图中进行Tag添加
	UPROPERTY(BlueprintReadWrite,Category="Warrior|Combat`")
	FGameplayTag CurrentEquippedWeaponTag;

	//通过CurrentEquippedWeaponTag检索当前装备的Weapon
	UFUNCTION(Blueprintable,Category="Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;

	//转换武器碰撞状态，设置一个Bool变量，true->QueryOnly false->NoCollision
	UFUNCTION(BlueprintCallable,Category="Warrior|Combat")
	void ToggleWeaponCollision(bool bShouldEnable,EToggleDamageType ToggleDamageType);

	//OnComponentBeginOverlap回调函数中自定义委托OnHitTarget的回调函数
	virtual void OnHitTargetActor(AActor*HitActor);
	//OnComponentEndOverlap回调函数中自定义委托OnWeaponPulledFromTarget的回调函数
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);
	
protected:
	//存储OverLap的对象，避免一次攻击中对单个物体多次触发伤害，会在EndOverlap中被清空重置
	UPROPERTY()
	TArray<AActor*>	OverLappedActors;
	
private:
	//Map映射：key：tag value：武器
	UPROPERTY()
	TMap<FGameplayTag,AWarriorWeaponBase*> CharacterCarriedWeaponMap;
	
	
};
