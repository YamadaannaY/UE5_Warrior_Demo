// Yu


#include "AbilitySystem/Abilities/HeroGameplayAbilit_HitReact.h"
#include "Characters/WarriorHeroCharacter.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/KismetMathLibrary.h"

void UHeroGameplayAbility_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	SelectMontageAndPlay(*TriggerEventData);
}

void UHeroGameplayAbility_HitReact::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo())
	{
		SkeletalMeshComponent->SetScalarParameterValueOnMaterials(TEXT("HitFxSwitch"),0);
	}
}

void UHeroGameplayAbility_HitReact::OnMontageFinished()
{
	CancelAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true);
}

void UHeroGameplayAbility_HitReact::SelectMontageAndPlay( const FGameplayEventData InPayLoad)
{
	float OutDistance;
	FGameplayTag DirectionTag=UWarriorFunctionLibrary::ComputeHitReactDirectionTag(InPayLoad.Instigator.Get(),GetHeroCharacterFromActorInfo(),OutDistance);
	
	if (DirectionTag==WarriorGamePlayTags::Shared_Status_HitReact_Front)
		{
		HitReactMontageToPlay = HitReactMontage_Front;
		}
	else if (DirectionTag==WarriorGamePlayTags::Shared_Status_HitReact_Back)
	{
		HitReactMontageToPlay = HitReactMontage_Back;
	}
	else if (DirectionTag==WarriorGamePlayTags::Shared_Status_HitReact_Left)
	{
		HitReactMontageToPlay = HitReactMontage_Left;
	}
	else if (DirectionTag==WarriorGamePlayTags::Shared_Status_HitReact_Right)
	{
		HitReactMontageToPlay = HitReactMontage_Right;
	}
	else
	{
		HitReactMontageToPlay = HitReactMontage_Front;
	}
	

	UAbilityTask_PlayMontageAndWait* Task=UAbilityTask_PlayMontageAndWait::
	CreatePlayMontageAndWaitProxy(
		this,
		TEXT("PlayMontageAndWait"), 
		HitReactMontageToPlay,
		1,
		NAME_None,
		true
		);

	Task->ReadyForActivation();
	Task->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontageFinished);
	Task->OnCancelled.AddUniqueDynamic(this,&ThisClass::OnMontageFinished);
	Task->OnBlendOut.AddUniqueDynamic(this,&ThisClass::OnMontageFinished);
	Task->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnMontageFinished);
	
	if (USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo())
	{
		SkeletalMeshComponent->SetScalarParameterValueOnMaterials(TEXT("HitFxSwitch"),1);
	}
}

FGameplayTag UHeroGameplayAbility_HitReact::ComputeHitReactDirectionTag(const AActor* InAttacker, AActor* InVictim,
	float& OutAngleDifference)
{
	check(InAttacker && InVictim);
	
	//获取前向向量和二者之间的单位方向向量
	const FVector VictimForward=InVictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalized=(InAttacker->GetActorLocation()-InVictim->GetActorLocation()).GetSafeNormal();

	//计算弧度值并转化为角度
	const float DotResult=FVector::DotProduct(VictimForward,VictimToAttackerNormalized);
	OutAngleDifference=UKismetMathLibrary::DegAcos(DotResult);
	
	//获得叉积
	const FVector CrossResult=FVector::CrossProduct(VictimForward,VictimToAttackerNormalized);
	//点积的z轴决定了角度值的正负（根据点积右手定则，垂直向量的方向正负就是二者角度的正负取值）
	if (CrossResult.Z<0.f)
	{
		OutAngleDifference*=-1.f;
	}
	//前方
	if (OutAngleDifference>=-45.f && OutAngleDifference<=45.f)
	{
		return WarriorGamePlayTags::Shared_Status_HitReact_Front;
	}
	//右方
	if (OutAngleDifference>45.f && OutAngleDifference<=135.f)
	{
		return WarriorGamePlayTags::Shared_Status_HitReact_Right;
	}
	//左方
	if (OutAngleDifference>135.f || OutAngleDifference<=-135.f)
	{
		return WarriorGamePlayTags::Shared_Status_HitReact_Back;
	}
	//后方
	if (OutAngleDifference<-45.f && OutAngleDifference>-135.f)
	{
		return WarriorGamePlayTags::Shared_Status_HitReact_Left;
	}

	return WarriorGamePlayTags::Shared_Status_HitReact_Front;
}


