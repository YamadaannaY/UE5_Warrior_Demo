// Yu

#include "AbilitySystem/AbilityTasks/AbilityTask_ExecuteOnTick.h"

UAbilityTask_ExecuteOnTick::UAbilityTask_ExecuteOnTick()
{
	bTickingTask=true;
}

UAbilityTask_ExecuteOnTick* UAbilityTask_ExecuteOnTick::ExecuteTaskOnTick(UGameplayAbility* OwningAbility)
{
	//NewAbilityTask是一个来自UAbilityTask基类的模板函数，用于安全地在GA上下文中创建任务实例。
	UAbilityTask_ExecuteOnTick* Node=NewAbilityTask<UAbilityTask_ExecuteOnTick>(OwningAbility);
	return Node;
}

void UAbilityTask_ExecuteOnTick::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		//自定义委托将帧时间进行广播，实现Tick级别的AbilityTask
		OnAbilityTaskTick.Broadcast(DeltaTime);
	}
	else
	{
		EndTask();
	}
}
