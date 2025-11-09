// Yu


#include "AbilitySystem/Abilities/GA_Hero_LightAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void UGA_Hero_LightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ClearTimer();
	FindIfSpecialTagAndSetMontage();
	PlayMontageAndDealFinished();
	ApplyDamage();
	HandleComboCount();
	SpecialAttackWithRage();
}

void UGA_Hero_LightAttack::PlayMontageAndDealFinished()
{
	UAbilityTask_PlayMontageAndWait* Task_PlayMontageAndWait=UAbilityTask_PlayMontageAndWait::
	CreatePlayMontageAndWaitProxy
	(this,TEXT("PlayMontageAndWait"),
		CurrentPlayingMontage,
		1,
		NAME_None
		);
	Task_PlayMontageAndWait->ReadyForActivation();
	Task_PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontagePlayingFinished);
	
}

void UGA_Hero_LightAttack::OnMontagePlayingFinished()
{
	CancelAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true);

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this,&ThisClass::ResetAttackComboCount);

	World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate,ResetComboTime, false);
}

void UGA_Hero_LightAttack::ClearTimer()
{
	UWorld* World = GetWorld();
	if (World && TimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
}

void UGA_Hero_LightAttack::FindIfSpecialTagAndSetMontage()
{
    AWarriorHeroCharacter* HeroCharacter = GetHeroCharacterFromActorInfo();
    if (!HeroCharacter)
    {
        return;
    }
	
    if (LightAttackMontages.Num() == 0)
    {
        CurrentPlayingMontage = nullptr;
        return;
    }

    bool bHaveSpecialTag = UWarriorFunctionLibrary::NativeDoesActorHaveTag(
        HeroCharacter, 
        WarriorGamePlayTags::Player_Status_JumpToFinisher
    );

    if (bHaveSpecialTag)
    {
    	CurrentLayerAttackComboCount=LightAttackMontages.Num();
    }
    UseComboCount = CurrentLayerAttackComboCount;
	
    if (LightAttackMontages.Contains(CurrentLayerAttackComboCount))
    {
        CurrentPlayingMontage = LightAttackMontages[CurrentLayerAttackComboCount];
        UE_LOG(LogTemp, Warning, TEXT("Selected montage with key %d: %s"), 
            CurrentLayerAttackComboCount, 
            *GetNameSafe(CurrentPlayingMontage));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Key %d not found in LightAttackMontages map!"), CurrentLayerAttackComboCount);
    	CurrentPlayingMontage= nullptr;
    }
}

void UGA_Hero_LightAttack::ApplyDamage()
{
	UAbilityTask_WaitGameplayEvent* Task_WaitGameplayEvent=UAbilityTask_WaitGameplayEvent::
	WaitGameplayEvent
	(this,WarriorGamePlayTags::Shared_Event_MeleeHit,nullptr,false,true);

	Task_WaitGameplayEvent->EventReceived.AddUniqueDynamic(this,&ThisClass::HandleApplyDamage);
}

void UGA_Hero_LightAttack::HandleApplyDamage(FGameplayEventData InPayLoad)
{
	const AActor* LocalTargetActor=InPayLoad.Target.Get();
	ExecuteGameplayCueToOwnerWithParams(GameplayCueTag);
	
	float InWeaponBaseDamage=GetHeroCombatComponentFromActorInfo()->GetHeroCurrentEquippedWeaponDamageAtLevel(GetAbilityLevel());
	FGameplayEffectSpecHandle DamageSpecHandle=MakeHeroDamageEffectSpecHandle(DamageGameplayEffectClass,InWeaponBaseDamage,WarriorGamePlayTags::Player_SetByCaller_AttackType_Light,UseComboCount);

	NativeApplyEffectSpecHandleToTarget(const_cast<AActor*>(LocalTargetActor),DamageSpecHandle);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(const_cast<AActor*>(LocalTargetActor),WarriorGamePlayTags::Shared_Event_HitReact,InPayLoad);

	ApplyGameplayEffectToOwner(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),GainRageGameplayEffectClass.GetDefaultObject(),GetAbilityLevel(),1);
}

void UGA_Hero_LightAttack::ResetAttackComboCount()
{
	CurrentLayerAttackComboCount=1;
	UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(GetHeroCharacterFromActorInfo(),WarriorGamePlayTags::Player_Status_JumpToFinisher);
}

void UGA_Hero_LightAttack::ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	ASC->ExecuteGameplayCue(InGameplayTag, MakeBlockGamePlayCueParams());
}



FGameplayCueParameters UGA_Hero_LightAttack::MakeBlockGamePlayCueParams() const 
{
	//确定Cue参数：绑定位置为SkeletalMeshComponent
	USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();
	FGameplayCueParameters CueParams;
	CueParams.TargetAttachComponent=SkeletalMeshComponent;
	return CueParams;
}

void UGA_Hero_LightAttack::HandleComboCount()
{
	if (CurrentLayerAttackComboCount==LightAttackMontages.Num())
	{
		ResetAttackComboCount();
	}
	else
	{
		if (CurrentLayerAttackComboCount==LightAttackMontages.Num()-1)
		{
			//此时轻击位于第三段，赋予角色特殊Tag，此时触发轻击重击都可以直接激活最后一段攻击
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(GetHeroCharacterFromActorInfo(),WarriorGamePlayTags::Player_Status_JumpToFinisher);
		}
		CurrentLayerAttackComboCount++;
	}
}

void UGA_Hero_LightAttack::SpecialAttackWithRage()
{
	if (UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(),WarriorGamePlayTags::Player_Status_Rage_Activating))
	{
		WhileRageActive();
	}
}

void UGA_Hero_LightAttack::WhileRageActive()
{
	Debug::Print("base class");
}






