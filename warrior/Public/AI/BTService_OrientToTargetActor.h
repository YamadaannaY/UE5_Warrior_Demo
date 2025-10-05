// Yu

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_OrientToTargetActor.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UBTService_OrientToTargetActor : public UBTService
{
	GENERATED_BODY()

	UBTService_OrientToTargetActor();

	//~Begin UBTNode Interface
	
	//为了确保BTService知道它的KeySelector对应黑板里的哪个Key，获取UBlackboardData*BBAsset，并将KeySelector进行绑定
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	//获得一个Description
	virtual FString GetStaticDescription() const override;
	//~End UBTNode Interface

	//如果该节点处于活动状态则每帧执行
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere,Category="Target")
	FBlackboardKeySelector InTargetActorKey;
	
	UPROPERTY(EditAnywhere,Category="Target")
	float RotationInterSpeed;
	
};
