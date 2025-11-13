// Yu


#include "AbilitySystem/Abilities/GA_Hero_SpecialAbility_HeavyAxe.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorGamePlayTags.h"
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

	if (!DamageSpecHandle.IsValid())
	{
		return ;
	}

	const AActor* LocalTargetActor=InPayLoad.Target.Get();
	NativeApplyEffectSpecHandleToTarget(const_cast<AActor*>(LocalTargetActor),DamageSpecHandle);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(const_cast<AActor*>(LocalTargetActor),WarriorGamePlayTags::Shared_Event_HitReact,InPayLoad);

	
}
