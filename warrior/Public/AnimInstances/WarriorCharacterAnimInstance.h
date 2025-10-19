// Yu

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include "WarriorCharacterAnimInstance.generated.h"

class AWarriorBaseCharacter;
class  UCharacterMovementComponent;

/**
 * Character 的ABP类。
 */
UCLASS()
class WARRIOR_API UWarriorCharacterAnimInstance : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()

public:
	//~Begin AnimInstance Interface
	
	//初始化OwingCharacter和 OwingMovementComponent
	virtual void NativeInitializeAnimation() override;

	//在GameThread执行，可能受游戏逻辑阻塞,而在专用的WorkerThread执行，不受GameThread卡顿影响，同时也减小了游戏线程负担
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	//~End AnimInstance Interface
	
protected:
	UPROPERTY()
	AWarriorBaseCharacter* OwingCharacter;
	
	UPROPERTY()
	UCharacterMovementComponent* OwingMovementComponent;
	
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="AnimData|LocomotionData")
	float GroundSpeed;

	//是否具有加速度，从而判断idle与jog的状态转换
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="AnimData|LocomotionData")
	bool bHasAcceleration;

	//判断旋转
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="AnimData|LocomotionData")
	float LocalMotionDirection;
	
};
