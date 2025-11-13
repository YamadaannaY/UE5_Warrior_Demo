// Yu


#include "AbilitySystem/Abilities/GA_Hero_UnequipWeapon.h"
#include "EnhancedInputSubsystems.h"
#include "WarriorDebugHelper.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Components/UI/HeroUIComponent.h"
#include "Conrtroller/WarriorHeroController.h"
#include "Items/Weapon/WarriorHeroWeapon.h"

void UGA_Hero_UnequipWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	PlayMontageAndWaitEventUnEquip();
	
}

void UGA_Hero_UnequipWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnTimerFinished, 0.01f, false);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Hero_UnequipWeapon::PlayMontageAndWaitEventUnEquip()
{
	UAbilityTask_PlayMontageAndWait* Task_PlayMontageAndWait=UAbilityTask_PlayMontageAndWait::
	CreatePlayMontageAndWaitProxy(
		this,
		TEXT("PlayMontageAndWait"),
		PlayUnequipMontage,
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
		EventUnEquipWeaponTag,
		nullptr,
		false,
		true
		);

	Task_WaitGameplayEvent->EventReceived.AddUniqueDynamic(this,&ThisClass::AttachWeapon);
	Task_WaitGameplayEvent->ReadyForActivation();
}

void UGA_Hero_UnequipWeapon::PlayMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true,true);
}

void UGA_Hero_UnequipWeapon::OnTimerFinished()
{
	//取消动画层,由于蓝图动画更新顺序与cpp不同，蓝图在动画层取消链接之后重新编译状态机，过渡完毕后再更新视觉，但是cpp
	//会先更新视觉，在下一帧才重新编译状态机，不可避免有一帧的僵直。
	
	GetOwningComponentFromActorInfo()->UnlinkAnimClassLayers(GetHeroCombatComponentFromActorInfo()->GetHeroCarriedWeaponByTags(CarriedWeaponTagToUse)
->HeroWeaponData.WeaponAnimLayerToLink.Get());
}


void UGA_Hero_UnequipWeapon::AttachWeapon(FGameplayEventData InPayload)
{
	//将当前装备Tag置为空
	GetHeroCombatComponentFromActorInfo()->CurrentEquippedWeaponTag=FGameplayTag();
	
	AWarriorHeroWeapon* HeroWeapon=GetHeroCombatComponentFromActorInfo()->GetHeroCarriedWeaponByTags(CarriedWeaponTagToUse);
	if (HeroWeapon)
	{
		USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();
		
		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(
		EAttachmentRule::SnapToTarget,    // Location Rule
		EAttachmentRule::KeepRelative,    // Rotation Rule  
		EAttachmentRule::KeepWorld,		  // Scale Rule
		true                              // Weld Simulated Bodies
		);
		
		HeroWeapon->AttachToComponent(SkeletalMeshComponent,AttachmentRules,SocketName);
		HandleUnEquipWeapon(HeroWeapon);
	}
	else
	{
		Debug::Print(TEXT("weapon error"));
	}
}

void UGA_Hero_UnequipWeapon::HandleUnEquipWeapon(AWarriorHeroWeapon* InWeaponToUnEquip)
{
	const ULocalPlayer* LocalPlayer=GetHeroControllerFromActorInfo()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	Subsystem->RemoveMappingContext(InWeaponToUnEquip->HeroWeaponData.WeaponInputMappingContext);

	TArray<FGameplayAbilitySpecHandle> OutGrantedAbilitySpecHandles=InWeaponToUnEquip->GetGrantedAbilitySpecHandles();
	GetWarriorAbilitySystemComponentFromActorInfo()->RemoveGrantedHeroWeaponAbilities(OutGrantedAbilitySpecHandles);

	UpdateUI(InWeaponToUnEquip);
}

void UGA_Hero_UnequipWeapon::UpdateUI(AWarriorHeroWeapon* InWeaponToUnEquip)
{
	GetHeroCharacterFromActorInfo()->GetHeroUIComponent()->OnEquippedWeaponChanged.Broadcast(nullptr);

	for (FWarriorHeroSpecialAbilitySet HeroSpecialAbilities : InWeaponToUnEquip->HeroWeaponData.SpecialWeaponAbilities)
	{
		//将所有UI图标进行更新，将其取消
		GetHeroCharacterFromActorInfo()->GetHeroUIComponent()->OnAbilityIconSlotUpdated.Broadcast(HeroSpecialAbilities.InputTag,nullptr);
	}
}


