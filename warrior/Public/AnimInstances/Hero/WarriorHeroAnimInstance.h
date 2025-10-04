// Yu

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorCharacterAnimInstance.h"
#include "Characters/WarriorHeroCharacter.h"
#include "WarriorHeroAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorHeroAnimInstance : public UWarriorCharacterAnimInstance
{
	GENERATED_BODY()
public:
	//当动画蓝图第一次初始化（也就是这个AnimInstance关联到某个SkeletalMeshComponent上时），
	//引擎会调用NativeInitializeAnimation()。这里复写使OwingCharacter指向的是HeroCharacter
	virtual void NativeInitializeAnimation() override;

	//动画的每帧调用
	//利用EnterRelaxStateThreshold和IdleElpasedTime判断bShouldEnterRelaxState状态
	//bShouldEnterRelaxState是状态机判断从IDLE到Relax的依据
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
protected:
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="AnimData|Refrences")
	AWarriorHeroCharacter* OwingHeroCharacter;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="AnimData|LocomotionData")
	bool bShouldEnterRelaxState;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="AnimData|LocomotionData")
	float EnterRelaxStateThreshold =5.f;

	float IdleElpasedTime=0.f;
};
