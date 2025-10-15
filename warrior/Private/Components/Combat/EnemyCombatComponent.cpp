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
	//两个判断：1.是否正在进行格挡 2.此次攻击是否不可阻挡
	const bool  bIsPlayerBlocking=UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitActor,WarriorGamePlayTags::Player_Status_Blocking);
	const bool bIsAttackUnblockable=UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetMyOwningPawn(),WarriorGamePlayTags::Enemy_Status_UnBlockable);
	if (bIsPlayerBlocking && !bIsAttackUnblockable)
	{
		//判断格挡成功
		bIsValidBlock=UWarriorFunctionLibrary::IsValidBlock(GetMyOwningPawn(),HitActor);
	}
	//SendGameplayEventToActor需要的Data参数
	FGameplayEventData EventData;
	EventData.Instigator=GetMyOwningPawn();
	EventData.Target=HitActor;
	
	if (bIsValidBlock)
	{
		//将格挡成功Tag返回。
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor,WarriorGamePlayTags::Player_Event_SuccessfulBlock,EventData);
	}
	
	else
	{
		//没有进行格挡或格挡失败，触发MeleeGA
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetMyOwningPawn(),WarriorGamePlayTags::Shared_Event_MeleeHit,EventData);
	}
}
