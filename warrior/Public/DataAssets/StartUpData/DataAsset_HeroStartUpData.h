// Yu

#pragma once

#include "CoreMinimal.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "DataAsset_HeroStartUpData.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UDataAsset_HeroStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()
public:
	//继承父类，在生成没有Tag的基础能力(Reactive属于利用EventTag触发)之后进行拓展，生成了Player专属的拥有Tag、需要输入触发的能力
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel=1)	override;
private:
	//存储AbilitySet的数组
	UPROPERTY(EditDefaultsOnly,Category="StartUpData",meta=(TitleProperty="InputTag"))
	TArray<FWarriorHeroAbilitySet>   HeroStartUpAbilitySets;	
};
