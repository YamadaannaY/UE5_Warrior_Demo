// Yu


#include "AbilitySystem/Abilities/WarriorGA_SpawnWeaponBase.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "Items/Weapon/WarriorWeaponBase.h"

void UWarriorGA_SpawnWeaponBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* Owner=GetAvatarActorFromActorInfo();
	APawn* Instigator=Cast<APawn>(Owner);

	UWorld* World=GetWorld();
	if (!World)
	{
		return;
	}

	//SpawnActor时为Actor赋予的参数
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner=Owner;
	SpawnParameters.Instigator=Instigator;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AWarriorWeaponBase* SpawnedWeapon=World->SpawnActor<AWarriorWeaponBase>(WeaponClassToSpawn,FVector::ZeroVector,FRotator::ZeroRotator,SpawnParameters);
	
	if (!SpawnedWeapon)
	{
		CancelAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true);
	}
	
	USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();
	
	SpawnedWeapon->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::KeepRelativeTransform,SocketNameToAttachTo);

	UPawnCombatComponent* CombatComponent=GetPawnCombatComponentFromActorInfo();
	CombatComponent->RegisterSpawnedWeapon(WeaponTagToRegister,SpawnedWeapon,RegisterAsEquipped);

	SpawnedWeapon->WeaponMesh->SetScalarParameterValueOnMaterials("DissolveAmount",0);

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
	
}
