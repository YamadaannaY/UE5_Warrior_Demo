// Yu

#pragma once
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "WarriorStructTypes.generated.h"


class UInputMappingContext;
class UWarriorHeroLinkedAnimLayer;
class UWarriorGameplayAbility;
class UInputAction;

//存储角色拥有的依赖输入的能力和其对应的Tag，内置一个IsValid()进行Tag判断，Tag通过能力激活时动态添加
USTRUCT(Blueprintable)
struct FWarriorHeroAbilitySet
{
	GENERATED_BODY()
	//依赖输入触发的Tag
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(Categories="InputTag"))
	FGameplayTag InputTag;

	//要生成的角色能力
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	//存储WarriorGameplayAbility类而非实例，在编辑器中进行实例化
	TSubclassOf<UWarriorGameplayAbility> AbilityToGrant;

	//如果Tag和能力都有效则True，否则False
	bool IsValid() const;
};

USTRUCT(BlueprintType)
struct FWarriorHeroSpecialAbilitySet: public FWarriorHeroAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSoftObjectPtr<UMaterialInterface> SoftAbilityIconMaterial;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(Categories="Player.Cooldown"))
	FGameplayTag AbilityCooldownTag;
};


//存储HeroWeaponData，具有与其绑定的anim layer、MappingContext和Weapon Abilities
USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData
{
	GENERATED_BODY()

	//要链接的Anim Layer，将Montage动画成为Layer的前半部分实现。
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UWarriorHeroLinkedAnimLayer> WeaponAnimLayerToLink;

	//要Add的IMC，此Context拥有所有用武器才能实现IA，优先级设置为1高于默认IMC，在装备武器时进行覆盖
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext;

	//存储所有在EquipWeapon状态下角色应该拥有的GA
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarriorHeroAbilitySet> DefaultWeaponAbilities;

	//存储所有特殊武器能力
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarriorHeroSpecialAbilitySet> SpecialWeaponAbilities;

	//FScalableFloat 是 Unreal Engine 的一个封装浮点数类型，它不仅存储一个普通的浮点值，还支持 数据表（DataTable）/曲线表（Curve
	//用于曲线表中
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FScalableFloat WeaponBaseDamage;
	
	//软引用，使用异步加载加载图标
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> SoftWeaponIconTexture;
};

//存储Tag和对应IA的结构体变量
USTRUCT(BlueprintType)
struct FWarriorInputActionConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(Categories="InputTag"))
	FGameplayTag InputTag;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputAction* InputAction;

	bool IsValid() const
	{
		return InputTag.IsValid() && InputAction;
	}
};

