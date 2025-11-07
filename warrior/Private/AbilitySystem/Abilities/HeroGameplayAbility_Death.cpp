// Yu


#include "AbilitySystem/Abilities/HeroGameplayAbility_Death.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Conrtroller/WarriorHeroController.h"
#include "GameMode/WarriorSurvialGameMode.h"
#include "Kismet/GameplayStatics.h"

void UHeroGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!MontageToPlay.IsEmpty())
	{
		const int32 RandomIndex=FMath::RandRange(0, MontageToPlay.Num()-1);

		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
this,                                
		TEXT("PlayMontageAndWait"),            
		MontageToPlay[RandomIndex],                            
		1.0f,                                   
		NAME_None,                              
		true);

		PlayMontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontageFinished);
		PlayMontageTask->OnBlendOut.AddUniqueDynamic(this,&ThisClass::OnMontageFinished);
		PlayMontageTask->OnCancelled.AddUniqueDynamic(this,&ThisClass::OnMontageFinished);
		PlayMontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnMontageFinished);
		
		PlayMontageTask->ReadyForActivation();
	}
}

void UHeroGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	GetOwningComponentFromActorInfo()->bPauseAnims = true;
	GetHeroCharacterFromActorInfo()->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	AGameModeBase* GameMode=UGameplayStatics::GetGameMode(this);
	if (GameMode)
	{
		AWarriorSurvivalGameMode* SurvivalGameMode=Cast<AWarriorSurvivalGameMode>(GameMode);
		if (SurvivalGameMode)
		{
			SurvivalGameMode->OnSurvivalGameModeStateChanged.Broadcast(EWarriorSurvivalGameModeState::PlayerDied);
		}
	}

	AWarriorHeroController* PlayerController=GetHeroControllerFromActorInfo();
	if (!PlayerController)
	{
		return;
	}

	PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->bShowMouseCursor = true;
	
	UE_LOG(LogTemp, Warning, TEXT("Input mode set to UI Only"));
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UHeroGameplayAbility_Death::OnMontageFinished()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}
