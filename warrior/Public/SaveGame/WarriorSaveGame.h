// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WarriorTypes/WarriorEnumType.h"
#include "WarriorSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	EWarriorGameDifficulty SavedCurrentDifficulty;
	
	
};
