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
	//将StartUpData中OnGiven和被动能力的Spec和GE以GrantAbility()/ApplyGameplayEffectToSelf赋予ASC
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel=1);
protected:
	//一旦被GiveAbility就触发的能力（如SpawnWeapon、DrawOverlayWidget）
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ActivateOnGivenAbilities;
	
	//响应式能力，依赖GameEvent或OnAdded触发
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ReactiveAbilities;

	//需要在游戏开始被实现的GE，绑定了数据并插入曲线表值，目前有两个，StratUpGE：表示初始数据，StaticGE:表示进行过程中的动态数据（在GEE中被改变）
	UPROPERTY(EditDefaultsOnly,Category="StartUpData")
	TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffects;

	//工具函数：生成能力，将能力注册到ASC
	static void GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>> &InAbilitiesToGive,UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel);
	
};
