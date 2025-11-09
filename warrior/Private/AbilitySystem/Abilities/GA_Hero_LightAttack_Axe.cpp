// Yu


#include "AbilitySystem/Abilities/GA_Hero_LightAttack_Axe.h"

#include "WarriorDebugHelper.h"
#include "Characters/WarriorHeroCharacter.h"
#include "WarriorGamePlayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapon/WarriorHeroWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Items/WarriorProjectileBase.h"

void UGA_Hero_LightAttack_Axe::WhileRageActive()
{
	UAbilityTask_WaitGameplayEvent* Task_WaitGameplayEvent=UAbilityTask_WaitGameplayEvent::
	WaitGameplayEvent(
		this,
		WarriorGamePlayTags::Shared_Event_SpawnProjectile,
		nullptr,
		false,
		true
		);

	Task_WaitGameplayEvent->EventReceived.AddUniqueDynamic(this,&ThisClass::SpawnProjectile);
	Task_WaitGameplayEvent->ReadyForActivation();
}

void UGA_Hero_LightAttack_Axe::SpawnProjectile(FGameplayEventData InPayLoad)
{
	AWarriorHeroWeapon* HeroWeapon=GetHeroCombatComponentFromActorInfo()->GetHeroCarriedEquippedWeapon();
	FTransform WeaponTransform=HeroWeapon->WeaponMesh->GetSocketTransform(FName("RageSlashSocket"),RTS_World);

	FVector ForwardVector=GetHeroCharacterFromActorInfo()->GetActorForwardVector();
	float ZValue=UKismetMathLibrary::MakeRotFromX(ForwardVector).Yaw;

	float  BaseDamage=GetHeroCombatComponentFromActorInfo()->GetHeroCurrentEquippedWeaponDamageAtLevel(GetAbilityLevel());
	
	UWorld* World=GetWorld();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParameters.Instigator=GetHeroCharacterFromActorInfo();
	SpawnParameters.Owner=GetHeroCharacterFromActorInfo();

	FVector SpawnLocation = WeaponTransform.GetLocation();
	FRotator SpawnRotation = FRotator(WeaponTransform.GetRotation().X, 0.0f, ZValue);
	AWarriorProjectileBase* SpawnedProjectile = World->SpawnActor<AWarriorProjectileBase>(HeroSlash, SpawnLocation,SpawnRotation,SpawnParameters);
	if (SpawnedProjectile)
	{
		Debug::Print("work");
	}
	SpawnedProjectile->ProjectileDamageEffectSpecHandle=MakeHeroDamageEffectSpecHandle(GameplayEffectClass,BaseDamage,WarriorGamePlayTags::Player_SetByCaller_AttackType_Light,3);
}
