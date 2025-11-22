// Yu


#include "AbilitySystem/Abilities/GA_Hero_EquipWeapon.h"
#include "AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h"
#include "EnhancedInputSubsystems.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Components/UI/HeroUIComponent.h"
#include "Conrtroller/WarriorHeroController.h"
#include "Items/Weapon/WarriorHeroWeapon.h"

void UGA_Hero_EquipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	PlayMontageAndWaitEventEquip();
	
}

void UGA_Hero_EquipWeapon::PlayMontageAndWaitEventEquip()
{
	UAbilityTask_PlayMontageAndWait* Task_PlayMontageAndWait=UAbilityTask_PlayMontageAndWait::
	CreatePlayMontageAndWaitProxy(
		this,
		TEXT("PlayMontageAndWait"),
		PlayMontage,
		1,
		NAME_None
		);

	Task_PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this,&ThisClass::PlayMontageCompleted);
	Task_PlayMontageAndWait->OnInterrupted.AddUniqueDynamic(this,&ThisClass::PlayMontageCompleted);
	Task_PlayMontageAndWait->OnBlendOut.AddUniqueDynamic(this,&ThisClass::PlayMontageCompleted);
	Task_PlayMontageAndWait->OnCancelled.AddUniqueDynamic(this,&ThisClass::PlayMontageCompleted);

	Task_PlayMontageAndWait->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* Task_WaitGameplayEvent=UAbilityTask_WaitGameplayEvent::
	WaitGameplayEvent(
		this,
		EventEquipWeaponTag,
		nullptr,
		false,
		true
		);

	Task_WaitGameplayEvent->EventReceived.AddUniqueDynamic(this,&ThisClass::AttachWeapon);
	Task_WaitGameplayEvent->ReadyForActivation();
}

void UGA_Hero_EquipWeapon::PlayMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true,true);
}

void UGA_Hero_EquipWeapon::AttachWeapon(FGameplayEventData InPayload)
{
	AWarriorHeroWeapon* HeroWeapon=GetHeroCombatComponentFromActorInfo()->GetHeroCarriedWeaponByTags(CurrentEquipWeaponTag);
	USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(
	EAttachmentRule::SnapToTarget,    // Location Rule
	EAttachmentRule::KeepRelative,    // Rotation Rule  
	EAttachmentRule::KeepWorld,		  // Scale Rule
	true                           // Weld Simulated Bodies
);
	
	HeroWeapon->AttachToComponent(SkeletalMeshComponent,AttachmentRules,SocketName);
	HandleEquipWeapon(HeroWeapon);
}

void UGA_Hero_EquipWeapon::HandleEquipWeapon(AWarriorHeroWeapon* InWeaponToEquip)
{
	FWarriorHeroWeaponData CacheWeaponData=InWeaponToEquip->HeroWeaponData;

	GetOwningComponentFromActorInfo()->LinkAnimClassLayers(CacheWeaponData.WeaponAnimLayerToLink.Get());
	
	const ULocalPlayer* LocalPlayer=GetHeroControllerFromActorInfo()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	Subsystem->AddMappingContext(CacheWeaponData.WeaponInputMappingContext,1);

	TArray<FGameplayAbilitySpecHandle> OutGrantedAbilitySpecHandles;
	GetWarriorAbilitySystemComponentFromActorInfo()->GrantHeroWeaponAbilities(CacheWeaponData.DefaultWeaponAbilities,CacheWeaponData.SpecialWeaponAbilities,GetAbilityLevel(),OutGrantedAbilitySpecHandles);

	//将数组存储到Weapon类本地变量中存储
	InWeaponToEquip->AssignGrantedAbilitySpecHandles(OutGrantedAbilitySpecHandles);

	UpdateUIAndCalRemainingTime(InWeaponToEquip);

	GetHeroCombatComponentFromActorInfo()->CurrentEquippedWeaponTag=CurrentEquipWeaponTag;
}

void UGA_Hero_EquipWeapon::UpdateUIAndCalRemainingTime(AWarriorHeroWeapon* InWeaponToEquip)
{
	FWarriorHeroWeaponData CacheWeaponData=InWeaponToEquip->HeroWeaponData;

	UHeroUIComponent* HeroUIComponent=GetHeroUIComponentFromActorInfo();
	HeroUIComponent->OnEquippedWeaponChanged.Broadcast(CacheWeaponData.SoftWeaponIconTexture);

	for (FWarriorHeroSpecialAbilitySet HeroSpecialAbility:CacheWeaponData.SpecialWeaponAbilities)
	{
		HeroUIComponent->OnAbilityIconSlotUpdated.Broadcast(HeroSpecialAbility.InputTag,HeroSpecialAbility.SoftAbilityIconMaterial);

		float TotalCoolDownTime=0;
		float RemainingCoolDownTime=0;
		
		bool bIsGetRemainingCooldown=GetAbilityRemainingCoolDownByTag(HeroSpecialAbility.AbilityCooldownTag,TotalCoolDownTime,RemainingCoolDownTime);
		
		if (bIsGetRemainingCooldown)
		{
			HeroUIComponent->OnAbilityCooldownBegin.Broadcast(HeroSpecialAbility.InputTag,TotalCoolDownTime,RemainingCoolDownTime);
		}
	}
}


