// Yu


#include "AbilitySystem/Abilities/HeroGameplayAbility_Sprint.h"

#include "Characters/WarriorHeroCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

void UHeroGameplayAbility_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UCharacterMovementComponent* CharacterMovement=GetHeroCharacterFromActorInfo()->GetCharacterMovement();
	if (CharacterMovement)
	{
		CachedMaxWalkSpeed=CharacterMovement->MaxWalkSpeed;
		CharacterMovement->MaxWalkSpeed=SprintWalkSpeed;
	}
}

void UHeroGameplayAbility_Sprint::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	UCharacterMovementComponent* CharacterMovement=GetHeroCharacterFromActorInfo()->GetCharacterMovement();
	CharacterMovement->MaxWalkSpeed=CachedMaxWalkSpeed;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
