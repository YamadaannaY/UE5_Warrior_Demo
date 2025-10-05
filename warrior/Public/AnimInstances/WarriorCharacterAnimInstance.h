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
	//初始化OwingCharacter和 OwingMovementComponent
	virtual void NativeInitializeAnimation() override;

	//在动画线程中计算GroundSpeed和bHasAcceleration，因为这两个值只在动画中被需要，所以不需要在游戏进程中实时需要
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	
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
