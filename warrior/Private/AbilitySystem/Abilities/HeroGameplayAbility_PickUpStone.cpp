// Yu


#include "AbilitySystem/Abilities/HeroGameplayAbility_PickUpStone.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Components/UI/HeroUIComponent.h"
#include "PickUps/WarriorStoneBase.h"

void UHeroGameplayAbility_PickUpStone::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* TriggerEventData)
{

	GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(true);
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_PickUpStone::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(false);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_PickUpStone::CollectStones()
{
	TArray<FHitResult> TraceHits;

	//创建自定义半径的Box在范围内进行碰撞并返回所有结果
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		GetHeroCharacterFromActorInfo()->GetActorLocation()+ -GetHeroCharacterFromActorInfo()->GetActorUpVector()+BoxTraceDistance,
		TraceBoxSize/2.f,
		(-GetHeroCharacterFromActorInfo()->GetActorUpVector()).ToOrientationRotator(),
		 StoneTraceChannel,
		 false,
		 TArray<AActor*>(),
		 bDrawDebugShape? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		 TraceHits,
		 true
		 );

	//将所有符合Stone类型的碰撞对象加入数组,同时判断是否有Stone在适用范围内，如果有就继续收集，如果没有说明离开stone拾取范围，直接清空当前存储数组
	uint8 StoneNum=0;
	
	for (const FHitResult& TraceHit:TraceHits)
	{
		if (AWarriorStoneBase* FoundStone=Cast<AWarriorStoneBase>(TraceHit.GetActor()))
		{
			CollectedStones.AddUnique(FoundStone);
			StoneNum++;
		}
	}
	
	if (StoneNum==0)
	{
		CollectedStones.Empty();
	}

	if (CollectedStones.IsEmpty())
	{
		CancelAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true);
	}
}

void UHeroGameplayAbility_PickUpStone::ConsumeStones()
{
	if (CollectedStones.IsEmpty())
	{
		CancelAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true);
		return;
	}

	//对所有数组中的Stone调用Consume（应用GE）,其中Destroy所有Actor，即自动清空数组
	for (AWarriorStoneBase* Stone:CollectedStones)
	{
		if (Stone)
		{
			Stone->Consume(GetWarriorAbilitySystemComponentFromActorInfo(),GetAbilityLevel());
		}
	}
}
