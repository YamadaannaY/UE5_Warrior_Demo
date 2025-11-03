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
	AWarriorGameMode();

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Gaem Settings")
	EWarriorGameDifficulty CurrentGameDifficulty;

public:
	FORCEINLINE EWarriorGameDifficulty GetCurrentGameDifficulty() const {return CurrentGameDifficulty;} 
};
