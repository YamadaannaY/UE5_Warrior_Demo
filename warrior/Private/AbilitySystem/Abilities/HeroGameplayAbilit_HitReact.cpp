// Yu


#include "AbilitySystem/Abilities/HeroGameplayAbilit_HitReact.h"
#include "Characters/WarriorHeroCharacter.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

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

