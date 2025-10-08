// Yu


#include "Components/Combat/EnemyCombatComponent.h"
#include "WarriorDebugHelper.h"
#include  "WarriorGamePlayTags.h"
#include "AbilitySystemBlueprintLibrary.h"


void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverLappedActors.Contains(HitActor))
	{
		return;
	}
	OverLappedActors.AddUnique(HitActor);

	bool bIsValidBlock=false;

	const bool bIsPlayerBlocking=false;
	const bool bIsMyAttackUnblockable=false;

	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		
	}

	FGameplayEventData EventData;
	EventData.Instigator=GetMyOwningPawn();
	EventData.Target=HitActor;
	
	if (bIsValidBlock)
	{
		
	}
	
	else
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetMyOwningPawn(),WarriorGamePlayTags::Shared_Event_MeleeHit,EventData);
	}
}
