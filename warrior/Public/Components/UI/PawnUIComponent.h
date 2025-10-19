// Yu

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include "PawnUIComponent.generated.h"

class UHeroUIComponent;
//声明一个单参数的多播委托，广播了各个参数相比于最大值的百分比，用于UI变化
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPercentChangedDelegate,float,NewPercent);

/**
 * 
 */
UCLASS()
class WARRIOR_API UPawnUIComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()

public:
	//~BlueprintAssignable 专用于多播委托，用于蓝图中绑定事件，

	//当前生命百分比变化的委托，一旦属性集中PostGameplayEffectExecute中生命值部分改变就BroadCast
	//在蓝图中BindEventTo，广播触发此Event
	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnCurrentHealthChanged;
};
