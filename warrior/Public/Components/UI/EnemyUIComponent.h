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
	//注册Widget到EnemyDrawnWidgets，Boss用，收集所有Widget，在Died中统一清理
	UFUNCTION(BlueprintCallable)
	void RegisterEnemyDrawWidget(UWarriorWidgetBase* InWidgetToRegister);

	//移除所有Widget
	UFUNCTION(BlueprintCallable)
	void RemoveEnemyDrawnWidgetsIfAny();
	
private:
	UPROPERTY()
	TArray<UWarriorWidgetBase*> EnemyDrawnWidgets;
	
};
