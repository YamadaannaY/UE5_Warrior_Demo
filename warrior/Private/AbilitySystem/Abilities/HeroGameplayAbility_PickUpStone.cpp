// Yu


#include "AbilitySystem/Abilities/HeroGameplayAbility_PickUpStone.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "PickUps/WarriorStoneBase.h"

void UHeroGameplayAbility_PickUpStone::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_PickUpStone::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
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

	//将所有符合Stone类型的碰撞对象加入数组
	for (const FHitResult& TraceHit:TraceHits)
	{
		if (AWarriorStoneBase* FoundStone=Cast<AWarriorStoneBase>(TraceHit.GetActor()))
		{
			CollectedStones.AddUnique(FoundStone);
		}
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

	//对所有数组中的Stone调用Consume（应用GE）
	for (AWarriorStoneBase* Stone:CollectedStones)
	{
		if (Stone)
		{
			Stone->Consume(GetWarriorAbilitySystemComponentFromActorInfo(),GetAbilityLevel());
		}
	}
	//消耗完后需要清空数组
	CollectedStones.Empty();
}
