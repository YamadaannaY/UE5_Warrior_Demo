// Yu


#include "AbilitySystem/Abilities/GA_Hero_HeavyAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void  UGA_Hero_HeavyAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ClearTimer();
	FindIfSpecialTagAndSetMontage();
	PlayMontageAndDealFinished();
	SpecialAttackWithRage();
	ApplyDamage();
	HandleComboCount();
}

void UGA_Hero_HeavyAttack::PlayMontageAndDealFinished()
{
	UAbilityTask_PlayMontageAndWait* Task_PlayMontageAndWait=UAbilityTask_PlayMontageAndWait::
	CreatePlayMontageAndWaitProxy
	(this,TEXT("PlayMontageAndWait"),
		CurrentPlayingMontage,
		1,
		NAME_None
		);
	Task_PlayMontageAndWait->OnCompleted.AddUniqueDynamic(this,&ThisClass::OnMontagePlayingFinished);
	Task_PlayMontageAndWait->ReadyForActivation();
	
}

//设置一个定时器，GA是PerActor,会保存当前段数，在定时范围内再次激活GA可以激活下一段，同时每一段GA都会清除当前Timer
// 否则ResetAttackComboCount
void UGA_Hero_HeavyAttack::OnMontagePlayingFinished()
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

void UGA_Hero_HeavyAttack::ClearTimer()
{
	UWorld* World = GetWorld();
	if (World && TimerHandle.IsValid())
	{
		//ClearAndInValidTimer
		World->GetTimerManager().ClearTimer(TimerHandle);
		TimerHandle.Invalidate();
	}
}

void UGA_Hero_HeavyAttack::FindIfSpecialTagAndSetMontage()
{
    AWarriorHeroCharacter* HeroCharacter = GetHeroCharacterFromActorInfo();
    if (!HeroCharacter)
    {
        return;
    }
	
    if (HeavyAttackMontages.Num() == 0)
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
    	CurrentLayerAttackComboCount=HeavyAttackMontages.Num();
    }
    UseComboCount = CurrentLayerAttackComboCount;
	
    if (HeavyAttackMontages.Contains(CurrentLayerAttackComboCount))
    {
        CurrentPlayingMontage = HeavyAttackMontages[CurrentLayerAttackComboCount];
        UE_LOG(LogTemp, Warning, TEXT("Selected montage with key %d: %s"), 
            CurrentLayerAttackComboCount, 
            *GetNameSafe(CurrentPlayingMontage));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Key %d not found in HeavyAttackMontages map!"), CurrentLayerAttackComboCount);
    	CurrentPlayingMontage= nullptr;
    }
}

void UGA_Hero_HeavyAttack::ApplyDamage()
{
	UAbilityTask_WaitGameplayEvent* Task_WaitGameplayEvent=UAbilityTask_WaitGameplayEvent::
	WaitGameplayEvent
	(this,WarriorGamePlayTags::Shared_Event_MeleeHit,nullptr,false,true);

	Task_WaitGameplayEvent->EventReceived.AddUniqueDynamic(this,&ThisClass::HandleApplyDamage);
	Task_WaitGameplayEvent->ReadyForActivation();
}

void UGA_Hero_HeavyAttack::HandleApplyDamage(FGameplayEventData InPayLoad)
{
	const AActor* LocalTargetActor=InPayLoad.Target.Get();
	ExecuteGameplayCueToOwnerWithParams(GameplayCueTag);
	
	float InWeaponBaseDamage=GetHeroCombatComponentFromActorInfo()->GetHeroCurrentEquippedWeaponDamageAtLevel(GetAbilityLevel());
	FGameplayEffectSpecHandle DamageSpecHandle=MakeHeroDamageEffectSpecHandle(DamageGameplayEffectClass,InWeaponBaseDamage,WarriorGamePlayTags::Player_SetByCaller_AttackType_Heavy,UseComboCount);

	NativeApplyEffectSpecHandleToTarget(const_cast<AActor*>(LocalTargetActor),DamageSpecHandle);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(const_cast<AActor*>(LocalTargetActor),WarriorGamePlayTags::Shared_Event_HitReact,InPayLoad);

	ApplyGameplayEffectToOwner(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),GainRageGameplayEffectClass.GetDefaultObject(),GetAbilityLevel(),1);
}

void UGA_Hero_HeavyAttack::ResetAttackComboCount()
{
	//超过Combo判定时间或者段数达到最后一段，回到第一段
	CurrentLayerAttackComboCount=1;
	UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(GetHeroCharacterFromActorInfo(),WarriorGamePlayTags::Player_Status_JumpToFinisher);
}

void UGA_Hero_HeavyAttack::ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	ASC->ExecuteGameplayCue(InGameplayTag, MakeBlockGamePlayCueParams());
}



FGameplayCueParameters UGA_Hero_HeavyAttack::MakeBlockGamePlayCueParams() const 
{
	//确定Cue参数：绑定位置为SkeletalMeshComponent
	USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();
	FGameplayCueParameters CueParams;
	CueParams.TargetAttachComponent=SkeletalMeshComponent;
	return CueParams;
}

void UGA_Hero_HeavyAttack::HandleComboCount()
{
	if (CurrentLayerAttackComboCount==HeavyAttackMontages.Num())
	{
		ResetAttackComboCount();
	}
	else
	{
	CurrentLayerAttackComboCount++;
	}
}

void UGA_Hero_HeavyAttack::SpecialAttackWithRage()
{
	if (UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(),WarriorGamePlayTags::Player_Status_Rage_Active))
	{
		WhileRageActive();
	}
}

void UGA_Hero_HeavyAttack::WhileRageActive()
{
}







