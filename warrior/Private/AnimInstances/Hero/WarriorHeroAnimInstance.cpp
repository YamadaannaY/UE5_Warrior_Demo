// Yu


#include "AnimInstances/Hero/WarriorHeroAnimInstance.h"

void UWarriorHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (OwingCharacter)
	{
		OwingHeroCharacter=Cast<AWarriorHeroCharacter>(OwingCharacter);
	}
}

void UWarriorHeroAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if (bHasAcceleration)
	{
		IdleElapsedTime=0.f;
		bShouldEnterRelaxState=false;
	}
	else
	{
		IdleElapsedTime+=DeltaSeconds;
		bShouldEnterRelaxState=(IdleElapsedTime>=EnterRelaxStateThreshold);
	}
}
