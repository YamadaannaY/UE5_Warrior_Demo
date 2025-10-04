// Yu

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarriorWidgetBase.generated.h"

class UEnemyUIComponent;
class UHeroUIComponent;
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	//引擎第一次初始化调用，在BP_OnOwingHeroUIComponentInitialized中传入HeroUIComponent
	virtual void NativeOnInitialized() override;

	//蓝图中实现函数，为HeroUIComponent进行初始化，绑定（HP、Rage的变化委托，传入NewPercent，然后设置Bar的percent，变化color）
	UFUNCTION(BlueprintImplementableEvent,meta=(DisplayName="On Owing Hero UIComponent Initialize"))
	void BP_OnOwingHeroUIComponentInitialized(UHeroUIComponent* HeroUIComponent);
	
	//蓝图中实现函数，为EnemyUIComponent进行初始化，绑定（HP的变化委托，传入NewPercent，然后设置Bar的percent，变化color）
	UFUNCTION(BlueprintImplementableEvent,meta=(DisplayName="On Owing Enemy UIComponent Initialize"))
	void BP_OnOwingEnemyUIComponentInitialized(UEnemyUIComponent* EnemyUIComponent);
public:
	//为BP_OnOwingEnemyUIComponentInitialized传入EnemyUIComponent
	UFUNCTION(BlueprintCallable)
	void InitEnemyCreatedWidget(AActor* OwingEnemyActor);
};
