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
		IdleElpasedTime=0.f;
		bShouldEnterRelaxState=false;
	}
	else
	{
		IdleElpasedTime+=DeltaSeconds;
		bShouldEnterRelaxState=(IdleElpasedTime>=EnterRelaxStateThreshold);
	}
}
