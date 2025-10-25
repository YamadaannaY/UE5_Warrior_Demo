// Yu

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "GameplayTagContainer.h"
#include "HeroUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate,TSoftObjectPtr<UTexture2D>,SoftWeaponIcon);
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityIconSlotUpdatedDelegate,FGameplayTag,AbilityInputTag,TSoftObjectPtr<UMaterialInterface>,SoftAbilityIconMaterial);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityCooldownBeginDelegate,FGameplayTag,AbilityInputTag,float,TotalCooldownTime,float,RemainingCooldownTime);
/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()
public:
	//只有Hero才有Rage,所以只设置在HeroUI中
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnCurrentRageChanged;

	//加载图标的委托回调
	UPROPERTY(BlueprintCallable,BlueprintAssignable)
	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChanged;

	//在Equip武器的时候调用，加载技能图标
	UPROPERTY(BlueprintCallable,BlueprintAssignable)
	FOnAbilityIconSlotUpdatedDelegate  OnAbilityIconSlotUpdated;

	//冷却时间的委托，在使用特殊技能后/装备武器时（用于技能冷却时装备武器时，需要加载冷却时间）
	UPROPERTY(BlueprintCallable,BlueprintAssignable)
	FOnAbilityCooldownBeginDelegate OnAbilityCooldownBegin;
};
