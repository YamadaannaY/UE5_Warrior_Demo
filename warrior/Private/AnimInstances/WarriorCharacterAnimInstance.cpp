// Yu


#include "AnimInstances/WarriorCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/WarriorBaseCharacter.h"
#include "KismetAnimationLibrary.h"

void UWarriorCharacterAnimInstance::NativeInitializeAnimation()
{
	//获得当前Owner的pawn类，转换为character类
	OwingCharacter=Cast<AWarriorBaseCharacter>(TryGetPawnOwner());
	
	if (OwingCharacter)
	{
		//获得character的CharacterMovement
		OwingMovementComponent=OwingCharacter->GetCharacterMovement();
	}
}

void UWarriorCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwingCharacter || !OwingMovementComponent)
	{
		return;
	}
	//ABP变量
	GroundSpeed=OwingCharacter->GetVelocity().Size2D();
	//ABP状态机参数
	bHasAcceleration=OwingMovementComponent->GetCurrentAcceleration().SizeSquared2D()> 0.f;

	LocalMotionDirection=UKismetAnimationLibrary::CalculateDirection(OwingCharacter->GetVelocity(),OwingCharacter->GetActorRotation());
}
