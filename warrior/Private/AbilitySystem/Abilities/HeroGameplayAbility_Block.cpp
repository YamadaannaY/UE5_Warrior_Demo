// Yu


#include "AbilitySystem/Abilities/HeroGameplayAbility_Block.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/RootMotionSource.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystemComponent.h"
#include "Characters/WarriorHeroCharacter.h"


void UHeroGameplayAbility_Block::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//激活Block同时记录时间
	HeroBlockActiveTime=UGameplayStatics::GetTimeSeconds(this);
	PlayMontageAndDealFinished();
}

void UHeroGameplayAbility_Block::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	//在结束能力时添加一个必要的时间流速判断
	if (UGameplayStatics::GetGlobalTimeDilation(this)!=1.f)
	{
		UGameplayStatics::SetGlobalTimeDilation(this,1.f);
	}
	GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(GameplayCueShieldTag);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_Block::AddGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	ASC->AddGameplayCue(InGameplayTag, MakeBlockGamePlayCueParams());
}

void UHeroGameplayAbility_Block::ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	ASC->ExecuteGameplayCue(InGameplayTag, MakeBlockGamePlayCueParams());
}



FGameplayCueParameters UHeroGameplayAbility_Block::MakeBlockGamePlayCueParams() const 
{
	//确定Cue参数：绑定位置为SkeletalMeshComponent
	USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();
	FGameplayCueParameters CueParams;
	CueParams.TargetAttachComponent=SkeletalMeshComponent;
	return CueParams;
}

void UHeroGameplayAbility_Block::StartResetJumpToFinishTimer()
{
	FTimerDynamicDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("ResetJumpToFinishState"));
	GetWorld()->GetTimerManager().SetTimer(
		   AbilityTimerHandle,
		   TimerDelegate,
		   0.3f, 
		   false 
	   );
}

void UHeroGameplayAbility_Block::ResetJumpToFinishState()
{
	AWarriorHeroCharacter* HeroCharacter=GetHeroCharacterFromActorInfo();
	UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(HeroCharacter,WarriorGamePlayTags::Player_Status_JumpToFinisher);
}

void UHeroGameplayAbility_Block::OnDelayCompleted()
{
	UGameplayStatics::SetGlobalTimeDilation(this,1.f);
	StartResetJumpToFinishTimer();
}


void UHeroGameplayAbility_Block::DealSuccessfulBlock(FGameplayEventData InPayLoad)
{
	//触发完美防御的条件
	if (UGameplayStatics::GetTimeSeconds(this)-HeroBlockActiveTime<=PerfectBlockJudgeTime)
	{
		bIsPerfectBlock=true;
	}
	
	if (AWarriorHeroCharacter* HeroCharacter=GetHeroCharacterFromActorInfo())
	{
		const FVector Location=HeroCharacter->GetActorLocation();
		const FVector LookAtLocation=InPayLoad.Instigator->GetActorLocation();

		const FRotator NewRotation=UKismetMathLibrary::FindLookAtRotation(Location,LookAtLocation);

		//防御成功后，转向目标方向
		HeroCharacter->SetActorRotation(NewRotation);

		//防御成功后一个受击后退动作Task
		FVector WorldDirection=HeroCharacter->GetActorForwardVector()*=-1;
		UAbilityTask_ApplyRootMotionConstantForce* ApplyRootMotionTask=UAbilityTask_ApplyRootMotionConstantForce::
		ApplyRootMotionConstantForce(
		this,
		NAME_None,
		WorldDirection,
		80,
		0.2,
		false,
		nullptr,
		ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity,
		FVector::ZeroVector,
		0,
		false
		);
		ApplyRootMotionTask->ReadyForActivation();

		//成功防御Cue
		ExecuteGameplayCueToOwnerWithParams(GameplayCueSuccessfulBlockTag);

		//完美防御造成额外特效
		if (bIsPerfectBlock)
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(HeroCharacter,WarriorGamePlayTags::Player_Status_JumpToFinisher);
			ExecuteGameplayCueToOwnerWithParams(GameplayCuePerfectBlockTag);

			UGameplayStatics::SetGlobalTimeDilation(this,0.2);

			UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this,0.08f );
			
			if (DelayTask)
			{
				DelayTask->OnFinish.AddDynamic(this, &ThisClass::OnDelayCompleted);
				DelayTask->ReadyForActivation();
			}
			bIsPerfectBlock=false;
		}
	}
}

void UHeroGameplayAbility_Block::OnMontageFinished()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
	StartResetJumpToFinishTimer();
}

void UHeroGameplayAbility_Block::PlayMontageAndDealFinished()
{
	checkf(HeroBlockMontage,TEXT("Forgot to assign Hero Block Montage To Play"));
	UAbilityTask_PlayMontageAndWait* PlayMontageTask=UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy
	(
		this,                                
	TEXT("PlayMontageAndWait"),            
		HeroBlockMontage,                            
		1.0f,                                   
		NAME_None,                              
		true
	);
	PlayMontageTask->ReadyForActivation();
	PlayMontageTask->OnCancelled.AddUniqueDynamic(this,&ThisClass::OnMontageFinished);
	PlayMontageTask->OnInterrupted.AddUniqueDynamic(this,&ThisClass::OnMontageFinished);

	//魔法盾出现
	AddGameplayCueToOwnerWithParams(GameplayCueShieldTag);

	WaitEventTask =UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
	  this,                                          
	  WarriorGamePlayTags::Player_Event_SuccessfulBlock,
	  nullptr,                                       
	  false,                                         
	  true                                          
  );
	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.AddUniqueDynamic(this,&ThisClass::DealSuccessfulBlock);
		WaitEventTask->ReadyForActivation();
	}
}

