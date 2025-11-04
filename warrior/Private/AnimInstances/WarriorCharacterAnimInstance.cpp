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
	GroundSpeed=OwingCharacter->GetVelocity().Size2D();
	
	//避免开方计算，直接计算这个向量的平方长度以减少消耗
	bHasAcceleration=OwingMovementComponent->GetCurrentAcceleration().SizeSquared2D()> 0.f;

	//计算角色相对一开始的旋转朝向
	LocalMotionDirection=UKismetAnimationLibrary::CalculateDirection(OwingCharacter->GetVelocity(),OwingCharacter->GetActorRotation());
}
