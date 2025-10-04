// Yu

#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"
class UWarriorGameplayAbility;
class UWarriorAbilitySystemComponent;
class UGameplayEffect;	
/**
 * 
 */
UCLASS()
class WARRIOR_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()
public:
	//将StartUpData中所有能力的Spec和GE以GrantAbility()/ApplyGameplayEffectToSelf赋予ASC
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel=1);
protected:
	//一旦被GiveAbility就触发的能力（如SpawnWeapon、DrawOverlayWidget）
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ActivateOnGivenAbilities;
	
	//响应式能力，依赖触发(如HitPause)
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ReactiveAbilities;

	//需要在游戏开始被实现的GE
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffects;

	//生成能力，将能力注册到ASC
	static void GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>> &InAbilitiesToGive,UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel);
	
};
