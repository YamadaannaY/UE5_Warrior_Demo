// Yu


#include "AbilitySystem/AbilityTasks/AbilityTask_ExecuteOnTick.h"

UAbilityTask_ExecuteOnTick::UAbilityTask_ExecuteOnTick()
{
	bTickingTask=true;
}

UAbilityTask_ExecuteOnTick* UAbilityTask_ExecuteOnTick::ExecuteTaskOnTick(UGameplayAbility* OwningAbility)
{
	//NewAbilityTask是一个模板函数（来自UAbilityTask基类），用于安全地在能力上下文中创建任务实例。
	UAbilityTask_ExecuteOnTick* Node=NewAbilityTask<UAbilityTask_ExecuteOnTick>(OwningAbility);
	return Node;
}

void UAbilityTask_ExecuteOnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	//this makes sure the ability is still active.
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		//自定义委托将帧时间进行广播
		OnAbilityTaskTick.Broadcast(DeltaTime);
	}
	else
	{
		//结束Task
		EndTask();
	}
}
