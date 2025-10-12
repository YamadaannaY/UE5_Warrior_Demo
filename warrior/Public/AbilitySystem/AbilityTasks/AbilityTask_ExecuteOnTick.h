// Yu

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_ExecuteOnTick.generated.h"

//创建一个单参数多播委托，将帧时间传递。
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnabilityTaskTickDelegate,float,DeltaTime);
/**
 * 
 */
UCLASS()
class WARRIOR_API UAbilityTask_ExecuteOnTick : public UAbilityTask
{
	GENERATED_BODY()
public:
	UAbilityTask_ExecuteOnTick();
	//在GameplayAbility内部生成一个UAbilityTask_ExecuteOnTick实例。
	UFUNCTION(BlueprintCallable, Category = "Ability|AbilityTasks",meta=(HidePin="OwningAbility",DefaultToSelf="OwningAbility",BlueprintInternalUseOnly="true"))
	static UAbilityTask_ExecuteOnTick* ExecuteTaskOnTick(UGameplayAbility* OwningAbility);
	//~Begin UGameplayTask Interface
	
	virtual void TickTask(float DeltaTime) override;
	//~End UGameplayTask Interface

	//委托实例。
	UPROPERTY(BlueprintAssignable)
	FOnabilityTaskTickDelegate OnAbilityTaskTick;
};
