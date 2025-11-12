// Yu

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "GameplayTagContainer.h"
#include "HeroUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate,TSoftObjectPtr<UTexture2D>,SoftWeaponIcon);
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityIconSlotUpdatedDelegate,FGameplayTag,AbilityInputTag,TSoftObjectPtr<UMaterialInterface>,SoftAbilityIconMaterial);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityCooldownBeginDelegate,FGameplayTag,AbilityInputTag,float,TotalCooldownTime,float,RemainingCooldownTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStoneInteractedDelegate,bool,bShouldDisplayInputKey);
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

	//在Equip武器的时候广播，加载技能图标
	UPROPERTY(BlueprintCallable,BlueprintAssignable)
	FOnAbilityIconSlotUpdatedDelegate  OnAbilityIconSlotUpdated;

	//冷却时间的委托，在使用有冷却时间的技能后进行广播
	UPROPERTY(BlueprintCallable,BlueprintAssignable)
	FOnAbilityCooldownBeginDelegate OnAbilityCooldownBegin;

	//出现输入提示UI，合适的广播时机应该是与StoneOverlap发生后的触发的回调GA中，此时还没有进行输入映射决定是否ConsumeStone
	UPROPERTY(BlueprintCallable,BlueprintAssignable)
	FOnStoneInteractedDelegate OnStoneInteracted;

};
