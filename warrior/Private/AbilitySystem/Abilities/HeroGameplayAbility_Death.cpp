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

		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,MontageToPlay[RandomIndex]);
		PlayMontageTask->OnCompleted.AddUniqueDynamic(this,&ThisClass::K2_EndAbility);
		PlayMontageTask->OnBlendOut.AddUniqueDynamic(this,&ThisClass::K2_EndAbility);
		PlayMontageTask->OnCancelled.AddUniqueDynamic(this,&ThisClass::K2_EndAbility);
		PlayMontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::K2_EndAbility);
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
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
