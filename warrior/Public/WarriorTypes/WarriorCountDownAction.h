// Yu

#pragma once

#include "CoreMinimal.h"
#include "WarriorTypes/WarriorEnumType.h"

/**
 * 
**/

class FWarriorCountDownAction:public FPendingLatentAction
{
public:
	FWarriorCountDownAction(
		float InTotalCountDownTime,
		float InUpdateInterval,
		float& InOutRemainingTime,
		EWarriorCountDownActionOutput& InCountDownOutput,
		const FLatentActionInfo& LatenInfo)
	:
		bNeedToCancel(false),
		TotalCountDownTime(InTotalCountDownTime),
		UpdateInterval(InUpdateInterval),
		OutRemainingTime(InOutRemainingTime),
		CountDownOutput(InCountDownOutput),
		ExecutionFunction(LatenInfo.ExecutionFunction),
		OutputLink(LatenInfo.Linkage),
		CallBackTarget(LatenInfo.CallbackTarget),
		ElapsedInterval(0.f),
		ElapsedTimeSinceStart(0.f)
	{
	}
	//在GameThread中世界会每帧更新，其中会对所有LatenAction调用UpdateOperation，即每帧更新
	virtual void UpdateOperation(FLatentResponse& Response) override;
	void CancelAction();

private:
	//是否Cancel
	bool bNeedToCancel;
	//延时时间
	float TotalCountDownTime;
	//更新频率
	float UpdateInterval;
	//输出引脚：剩余的时间
	float& OutRemainingTime;
	//输出引脚
	EWarriorCountDownActionOutput& CountDownOutput;
	//LatenInfo用于FinishAndTriggerIf的三个参数
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallBackTarget;
	//继上一次触发update后经过的时间周期
	float ElapsedInterval;
	float ElapsedTimeSinceStart;
};