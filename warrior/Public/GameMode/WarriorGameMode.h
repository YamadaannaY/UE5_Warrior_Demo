// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WarriorTypes/WarriorEnumType.h"
#include "WarriorGameMode.generated.h"
/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	//设置每帧执行Mode中逻辑
	AWarriorGameMode();

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Gaem Settings")
	EWarriorGameDifficulty CurrentGameDifficulty;

public:
	//获取存储的游戏难度
	FORCEINLINE EWarriorGameDifficulty GetCurrentGameDifficulty() const {return CurrentGameDifficulty;} 
};
