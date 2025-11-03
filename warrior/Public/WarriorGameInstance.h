// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/GameInstance.h"
#include "WarriorGameInstance.generated.h"

//LevelTag和Level
USTRUCT(BlueprintType)
struct FWarriorGameLevelSet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly,meta=(Category="GameData.Level"))
	FGameplayTag LevelTag;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> Level;

	bool IsValid() const
	{
		return LevelTag.IsValid() && !Level.IsNull();
	}
};

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	//allow custom GameInstances an opportunity to set up what it needs，为GameInstance初始化的位置
	virtual void Init() override;

protected:
	//预加载Map时逻辑
	virtual void OnPreLoadMap( const FString& MapName);
	//所有关卡及其Tag的集合
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<FWarriorGameLevelSet> GameLevelSets;

public:
	//通过Tag获得不同的World进行关卡转换
	UFUNCTION(BlueprintPure,meta=(GameplayTagFilter="GameData.Level"))
	TSoftObjectPtr<UWorld> GetGameLevelByTag(FGameplayTag InTag) const ;
};
