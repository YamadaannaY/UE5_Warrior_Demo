// Yu


#include "AbilitySystem/Abilities/HeroAbilityDir/HeroGA_Roll.h"
#include "MotionWarpingComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UHeroGA_Roll::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	ComputeRotateDirection();
	PlayRollMontage();
}

void UHeroGA_Roll::ComputeRotateDirection()
{
	FVector CacheRollingLocation=GetHeroCharacterFromActorInfo()->GetLastMovementInputVector().GetSafeNormal();
	FRotator CacheRollingRotation=UKismetMathLibrary::MakeRotFromX(CacheRollingLocation);
	
	UMotionWarpingComponent* HeroMotionWarpingComponent=GetHeroCharacterFromActorInfo()->GetMotionWarpingComponent();
	HeroMotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName("RollingDirection"),FVector::ZeroVector,CacheRollingRotation);

	//距离自定义实现
	const FVector LineTraceStart=GetHeroCharacterFromActorInfo()->GetActorLocation()+CacheRollingLocation*HeroRollingDistance;
	const FVector LineTraceEnd=GetHeroCharacterFromActorInfo()->GetActorUpVector()*-500+LineTraceStart;

	FHitResult LineTraceOutHit;
	UKismetSystemLibrary::LineTraceSingleForObjects
	(
		this,
		LineTraceStart,
		LineTraceEnd,
		RollLineTraceObjectType,
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		LineTraceOutHit,
		true
		);

	if (LineTraceOutHit.bBlockingHit)
	{
		HeroMotionWarpingComponent->AddOrUpdateWarpTargetFromLocation
		(FName("RollTargetLocation"),LineTraceOutHit.ImpactPoint);
	}
	else
	{
		HeroMotionWarpingComponent->RemoveWarpTarget(FName("RollTargetLocation"));
	}
	
}

void UHeroGA_Roll::PlayRollMontage()
{
	UAbilityTask_PlayMontageAndWait* Task_PlayMontageAndWait=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy
	(this,
		FName("PlayRollMontage"),
		RollMontage);

	Task_PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this,&ThisClass::RollMontageFinished);
	Task_PlayMontageAndWait->OnCancelled.AddUniqueDynamic(this,&ThisClass::RollMontageFinished);
	Task_PlayMontageAndWait->OnBlendOut.AddUniqueDynamic(this,&ThisClass::RollMontageFinished);
	Task_PlayMontageAndWait->OnInterrupted.AddUniqueDynamic(this,&ThisClass::RollMontageFinished);

	Task_PlayMontageAndWait->ReadyForActivation();
}

void UHeroGA_Roll::RollMontageFinished()
{
	EndAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true,false);
}
