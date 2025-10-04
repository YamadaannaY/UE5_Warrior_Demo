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

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//IPawnCombatInterface接口类存储的一个纯虚函数，被Character继承，在Base中为nullptr,在Base子类中为其UPawnCombatComponent
	virtual UPawnCombatComponent* GetPawnCombatComponent() const =0;
	
};
