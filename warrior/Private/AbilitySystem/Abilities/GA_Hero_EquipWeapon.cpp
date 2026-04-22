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
	
	UAbilityTask_PlayMontageAndWait* Task_PlayMontageAndWait=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,PlayMontage);
	Task_PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this,&ThisClass::K2_EndAbility);
	Task_PlayMontageAndWait->OnInterrupted.AddUniqueDynamic(this,&ThisClass::K2_EndAbility);
	Task_PlayMontageAndWait->OnBlendOut.AddUniqueDynamic(this,&ThisClass::K2_EndAbility);
	Task_PlayMontageAndWait->OnCancelled.AddUniqueDynamic(this,&ThisClass::K2_EndAbility);
	Task_PlayMontageAndWait->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitAttachWeaponEvent=UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,EventEquipWeaponTag);
	WaitAttachWeaponEvent->EventReceived.AddUniqueDynamic(this,&ThisClass::AttachWeapon);
	WaitAttachWeaponEvent->ReadyForActivation();
}

void UGA_Hero_EquipWeapon::AttachWeapon(FGameplayEventData InPayload)
{
	AWarriorHeroWeapon* HeroWeapon=GetHeroCombatComponentFromActorInfo()->GetHeroCarriedWeaponByTags(CurrentEquipWeaponTag);
	USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget,EAttachmentRule::KeepRelative,EAttachmentRule::KeepWorld,true);
	HeroWeapon->AttachToComponent(SkeletalMeshComponent,AttachmentRules,SocketName);
	
	HandleEquipWeapon(HeroWeapon);
}

void UGA_Hero_EquipWeapon::HandleEquipWeapon(AWarriorHeroWeapon* InWeaponToEquip)
{
	FWarriorHeroWeaponData CacheWeaponData=InWeaponToEquip->HeroWeaponData;

	GetOwningComponentFromActorInfo()->LinkAnimClassLayers(CacheWeaponData.WeaponAnimLayerToLink.Get());
	
	const ULocalPlayer* LocalPlayer=GetHeroControllerFromActorInfo()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	InputSubsystem->AddMappingContext(CacheWeaponData.WeaponInputMappingContext,1);

	TArray<FGameplayAbilitySpecHandle> OutGrantedAbilitySpecHandles;
	GetWarriorAbilitySystemComponentFromActorInfo()->GrantHeroWeaponAbilities(CacheWeaponData.DefaultWeaponAbilities,CacheWeaponData.SpecialWeaponAbilities,GetAbilityLevel(),OutGrantedAbilitySpecHandles);

	//将数组存储到Weapon类本地变量中存储方便卸下装备时删除
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


