// Yu


#include "AbilitySystem/Abilities/GA_Hero_SpecialAbility_HeavyAxe.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorDebugHelper.h"
#include "WarriorGamePlayTags.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UGA_Hero_SpecialAbility_HeavyAxe::HandleEventReceived(FGameplayEventData InPayLoad)
{
	FVector BoxTraceStart=GetHeroCharacterFromActorInfo()->GetActorLocation();
	FVector BoxTraceEnd=GetHeroCharacterFromActorInfo()->GetActorForwardVector()*200.f+BoxTraceStart;
	
	FRotator Orientation=UKismetMathLibrary::MakeRotFromX(GetHeroCharacterFromActorInfo()->GetActorForwardVector());
	
	TArray<FHitResult> TraceHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects
	(
		this,
		BoxTraceStart,
		BoxTraceEnd,
		BoxSize,
		Orientation,
		ObjectTypeQuery,
		false,
		TArray<AActor*>(),
		bDrawDebugShape? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		TraceHits,
		true
		);

	float InWeaponBaseDamage=GetHeroCombatComponentFromActorInfo()->GetHeroCurrentEquippedWeaponDamageAtLevel(GetAbilityLevel());
	FGameplayEffectSpecHandle DamageSpecHandle=MakeHeroDamageEffectSpecHandle(DamageGameplayEffectClass,InWeaponBaseDamage,WarriorGamePlayTags::Player_SetByCaller_AttackType_Heavy,HeavyUseComboCount);

	if (!DamageSpecHandle.IsValid() ||  TraceHits.IsEmpty())
	{
		Debug::Print("no spechandle or no tracehit");
		return ;
	}
	
	for (const FHitResult& TraceHit : TraceHits)
	{
		if (AWarriorEnemyCharacter* Enemy=Cast<AWarriorEnemyCharacter>(TraceHit.GetActor()))
		{
			NativeApplyEffectSpecHandleToTarget(Enemy,DamageSpecHandle);

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Enemy,WarriorGamePlayTags::Shared_Event_HitReact,InPayLoad);
		}
	}
}