// Yu

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToFaceTarget.generated.h"

/**
 * 
 */

//在BTTask运行期间，保存当前任务实例的运行时数据（即每个任务节点自己的“局部内存”）会被接口函数分配对应内存
struct FRotatorToFaceTargetTaskMemory
{
	//这两个变量会随着感知变化随时被更新或销毁，所以用弱指针存储避免悬空。
	TWeakObjectPtr<APawn> OwingPawn;
	TWeakObjectPtr<AActor> TargetActor;
	
	//辅助函数，判断弱引用是否还持有
	bool IsValid() const
	{
		return OwingPawn.IsValid() && TargetActor.IsValid();
	}
	//将弱指针释放，在成功将值传入HasReachingAnglePrecision时使用
	void Reset()
	{
		// Reset the weak pointer back to the null state
		OwingPawn.Reset();
		TargetActor.Reset();
	}
};

UCLASS()
class WARRIOR_API UBTTask_RotateToFaceTarget : public UBTTaskNode
{
	GENERATED_BODY()

	UBTTask_RotateToFaceTarget();

	//~Begin UBTNode Interface
	
	//将FBlackboardKeySelector初始化
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	
	//告诉行为树系统这个节点每个实例（运行时）需要多少内存来保存自己的状态数据。
	virtual uint16 GetInstanceMemorySize() const override;
	
	//注解
	virtual FString GetStaticDescription() const override;
	//~End UBTNode Interface

	//~Begin UBTTaskNode Interface
	
	//在Task被触发时执行的函数
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//在Task触发期间每帧执行的函数
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	//~End UBTTaskNode Interface

	//设置朝向的角度精度
	bool HasReachingAnglePrecision(APawn*QueryPawn,AActor* TargetActor) const;

	//面向Actor的角度精度限制
	UPROPERTY(EditAnywhere,Category="Face Target")
	float AnglePrecision;

	//转向速度
	UPROPERTY(EditAnywhere,Category="Face Target")
	float RotationInterSpeed;

	//TargetActor的参数变量
	UPROPERTY(EditAnywhere,Category="Face Target")
	FBlackboardKeySelector InTargetToFaceKey;
};
