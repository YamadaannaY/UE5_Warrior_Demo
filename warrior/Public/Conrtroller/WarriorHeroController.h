// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorHeroController.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorHeroController : public APlayerController,public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	//设置Controller Possess 对象的TeamID
	AWarriorHeroController();
	
	//~ Begin IGenericTeamAgentInterface Interface

	virtual FGenericTeamId GetGenericTeamId() const override;
	
	//~ End IGenericTeamAgentInterface Interface

private:
	FGenericTeamId HeroTeamId;

	
};
