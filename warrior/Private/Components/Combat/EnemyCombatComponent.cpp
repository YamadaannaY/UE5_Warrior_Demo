// Yu


#include "Components/Combat/EnemyCombatComponent.h"
#include "WarriorDebugHelper.h"
#include  "WarriorGamePlayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorFunctionLibrary.h"



void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverLappedActors.Contains(HitActor))
	{
		return;
	}
	OverLappedActors.AddUnique(HitActor);

	bool bIsValidBlock=false;
	const bool  bIsPlayerBlocking=UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitActor,WarriorGamePlayTags::Player_Status_Blocking);
	const bool bIsMyAttackUnblockable=false;

	if (bIsPlayerBlocking && !bIsMyAttackUnblockable)
	{
		bIsValidBlock=UWarriorFunctionLibrary::IsValidBlock(GetMyOwningPawn(),HitActor);
		
	}

	FGameplayEventData EventData;
	EventData.Instigator=GetMyOwningPawn();
	EventData.Target=HitActor;
	
	if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor,WarriorGamePlayTags::Player_Event_SuccessfulBlock,EventData);
	}
	
	else
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetMyOwningPawn(),WarriorGamePlayTags::Shared_Event_MeleeHit,EventData);
	}
}
