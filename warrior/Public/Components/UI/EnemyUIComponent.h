// Yu

#pragma once

#include "CoreMinimal.h"
#include "Components/UI/PawnUIComponent.h"
#include "EnemyUIComponent.generated.h"

class UWarriorWidgetBase;
/**
 * 
 */
UCLASS()
class WARRIOR_API UEnemyUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()
	
public:
	//注册Widget到EnemyDrawnWidgets
	UFUNCTION(BlueprintCallable)
	void RegisterEnemyDrawWidget(UWarriorWidgetBase* InWidgetToRegister);

	//移除所有Widget
	UFUNCTION(BlueprintCallable)
	void RemoveEnemyDrawnWidgetsIfAny();
	
private:
	UPROPERTY()
	TArray<UWarriorWidgetBase*> EnemyDrawnWidgets;
	
};
