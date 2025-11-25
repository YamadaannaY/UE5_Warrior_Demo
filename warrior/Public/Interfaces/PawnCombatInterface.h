// Yu

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnCombatInterface.generated.h"

class UPawnCombatComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPawnCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WARRIOR_API IPawnCombatInterface
{
	GENERATED_BODY()

public:
	//被Character继承重写，在Base中为nullptr,在Base子类中为具体的UPawnCombatComponent，此接口public使得其他类都可以轻松获取角色类的CombatComponent
	virtual UPawnCombatComponent* GetPawnCombatComponent() const =0;
	
};
