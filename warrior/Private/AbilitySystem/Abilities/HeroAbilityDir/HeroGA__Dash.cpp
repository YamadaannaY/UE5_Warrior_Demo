// Yu


#include "AbilitySystem/Abilities/HeroAbilityDir/HeroGA__Dash.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/PawnMovementComponent.h"

void UHeroGA__Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Dashing();
}

void UHeroGA__Dash::Dashing()
{
	ERootMotionFinishVelocityMode VelocityMode=ERootMotionFinishVelocityMode::ClampVelocity;
	
	float DashFinishedSpeed=500.f;
	if (GetHeroCharacterFromActorInfo()->GetMovementComponent()->GetMaxSpeed())
	{
		//将Dash结束速度设置为Movement的角色速度，否则是默认值500.f
		DashFinishedSpeed=GetHeroCharacterFromActorInfo()->GetMovementComponent()->GetMaxSpeed();
	}
	
	UAbilityTask_ApplyRootMotionConstantForce* Task_ApplyRootMotionConstantForce=UAbilityTask_ApplyRootMotionConstantForce::
	ApplyRootMotionConstantForce
	(
		this,
		NAME_None,
		GetDashDistance(),
		DashStrength,
		DashTime,
		false,   // 是否叠加
		nullptr, //力随时间变化的曲线
		VelocityMode,
		FVector::ZeroVector,
		DashFinishedSpeed,
		false
	);
	Task_ApplyRootMotionConstantForce->OnFinish.AddUniqueDynamic(this,&ThisClass::FinishDashing);
	//没有这一步Task只会执行而不会结束
	Task_ApplyRootMotionConstantForce->ReadyForActivation();

	//确定Cue参数：绑定位置为SkeletalMeshComponent
	USkeletalMeshComponent* SkeletalMeshComponent=GetOwningComponentFromActorInfo();
	FGameplayCueParameters CueParams;
	CueParams.TargetAttachComponent=SkeletalMeshComponent;
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	
	ASC->AddGameplayCue(DashGameplayCueTag,CueParams);
}

FVector UHeroGA__Dash::GetDashDistance()
{
	const APawn* HeroPawn=Cast<APawn>(GetHeroCharacterFromActorInfo());
	
	FVector DefaultDashVector=GetHeroCharacterFromActorInfo()->GetActorForwardVector();
	if (HeroPawn)
	{
		FVector DashVector=HeroPawn->GetLastMovementInputVector();
		if (DashVector.IsNearlyZero())
		{
			return DefaultDashVector;
		}
		else
		{
			return DashVector;
		}
	}
	
	return DefaultDashVector;
}

void UHeroGA__Dash::FinishDashing()
{
	GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(DashGameplayCueTag);

	EndAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true,false);
}


