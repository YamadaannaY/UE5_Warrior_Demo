// Yu


#include "Components/Combat/EnemyCombatComponent.h"
#include "Characters/WarriorEnemyCharacter.h"
#include  "WarriorGamePlayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorFunctionLibrary.h"
#include "Components/BoxComponent.h"


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
		//没有进行格挡或格挡失败，触发Shared_Event_MeleeHit
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetMyOwningPawn(),WarriorGamePlayTags::Shared_Event_MeleeHit,EventData);
	}
}

void UEnemyCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	const AWarriorEnemyCharacter* OwningEnemyCharacter=Cast<AWarriorEnemyCharacter>(GetOwner());
	check(OwningEnemyCharacter);

	//获取左右手Box
	UBoxComponent* LeftHandCollisionBox=OwningEnemyCharacter->GetLeftHandCollisionBox();
	UBoxComponent* RightHandCollisionBox=OwningEnemyCharacter->GetRightHandCollisionBox();
	check(LeftHandCollisionBox && RightHandCollisionBox);

	//根据传入参数设置左右手碰撞状态
	switch (ToggleDamageType)
	{
	case EToggleDamageType::LeftHand:
		LeftHandCollisionBox->SetCollisionEnabled(bShouldEnable?ECollisionEnabled::Type::QueryOnly :ECollisionEnabled::Type::NoCollision);
		break;
	case EToggleDamageType::RightHand:
		RightHandCollisionBox->SetCollisionEnabled(bShouldEnable?ECollisionEnabled::Type::QueryOnly :ECollisionEnabled::Type::NoCollision);
		break;
	default:
		 break;
	}

	if (!bShouldEnable)
	{
		OverLappedActors.Empty();
	}
}
