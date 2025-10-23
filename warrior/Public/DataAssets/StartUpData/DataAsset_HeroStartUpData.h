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
	//继承父类，在基础能力之后进行拓展，继续Give由Player专属的拥有Tag且需要输入映射进行触发的能力
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,int32 ApplyLevel=1)	override;
private:
	//存储AbilitySet的数组
	UPROPERTY(EditDefaultsOnly,Category="StartUpData",meta=(TitleProperty="InputTag"))
	TArray<FWarriorHeroAbilitySet>   HeroStartUpAbilitySets;	
};
