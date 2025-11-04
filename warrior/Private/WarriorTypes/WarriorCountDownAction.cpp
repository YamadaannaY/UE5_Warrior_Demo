// Yu


#include "WarriorTypes/WarriorCountDownAction.h"

void FWarriorCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	if (bNeedToCancel)
	{
		CountDownOutput=EWarriorCountDownActionOutput::Cancelled;
		//，再执行一次这个函数，此时输出的是cancelled，随后立即结束这个倒计时操作
		Response.FinishAndTriggerIf(true,ExecutionFunction,OutputLink,CallBackTarget);

		return;
	}
	//如果从开始计算的时间大于等于延时时间，则意味着完成
	if (ElapsedTimeSinceStart>= TotalCountDownTime)
	{
		CountDownOutput=EWarriorCountDownActionOutput::Completed;
		Response.FinishAndTriggerIf(true,ExecutionFunction,OutputLink,CallBackTarget);
		return;
	}

	//每隔UpdateInterval秒进行一次更新
	//原理：只在规定频率触发事件，而且ElapsedTimeSinceStart非常精准，减少的是固定值而不是我们时刻迭代的经过时间值。频率大于0时根据频率更新，频率小于0时实时更新
	//如果继上一次update后经过时间小于间隔时间，执行简单加法，性能极高
	if (ElapsedInterval < UpdateInterval)
	{
		ElapsedInterval += Response.ElapsedTime();
	}
	//此时叠加时间大于等于更新时间
	else
	{
		//累积这个函数进行更新的总时间
		ElapsedTimeSinceStart +=UpdateInterval > 0.f ? UpdateInterval : Response.ElapsedTime();
		//还剩下的延时时间等于总延时减掉当前的总时间
		OutRemainingTime = TotalCountDownTime - ElapsedTimeSinceStart;
		//触发一次Update
		Response.TriggerLink(ExecutionFunction,OutputLink,CallBackTarget);

		ElapsedInterval = 0.f;
	}
}

void FWarriorCountDownAction::CancelAction()
{
	bNeedToCancel=true;
}
