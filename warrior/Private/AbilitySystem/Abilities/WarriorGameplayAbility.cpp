// Yu


#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/PawnCombatComponent.h"


UPawnCombatComponent* UWarriorGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	// GetAvatarActorFromActorInfo():Returns the physical actor that is executing this ability.
	// 通过FindComponentByClass寻找到UPawnCombatComponent
	return  GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

UWarriorAbilitySystemComponent* UWarriorGameplayAbility::GetWarriorAbilitySystemComponentFromActorInfo() const
{
	//当前持有GA的Actor会自带FGameplayAbilityActorInfo：CurrentActorInfo：Actor*, MovementComponent*, AnimInstance, etc.
	return Cast<UWarriorAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}


void UWarriorGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	//如果被Give的能力为OnGiven（不依赖输入，没有tag，要求游戏开始就拥有的能力，eg：SpawnWeapon）类型
	//且Spec没有被触发（即没有使用此技能）则尝试触发此能力。
	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && ! Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}


void UWarriorGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const bool bReplicateEndAbility, const bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle) const
{
	//获取目标的ASC
	UAbilitySystemComponent* TargetASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	check(TargetASC && InSpecHandle.IsValid());
	//获取自己的ASC，并将作为参数的GE_Spec作用到目标ASC上,Apply意味着立即应用并激活这个GE
	//除此之外，ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data,TargetASC)对Source和Target进行了区分（OwnASC/TargetASC）
	return GetWarriorAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data,TargetASC);
	
}

//封装函数NativeApplyEffectSpecHandleToTarget并返回输出引脚
FActiveGameplayEffectHandle UWarriorGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle, EWarriorSuccessType& OutSuccessType) const 
{
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle=NativeApplyEffectSpecHandleToTarget(TargetActor,InSpecHandle);
	//GE是否成功应用.
	OutSuccessType=ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? EWarriorSuccessType::Successful : EWarriorSuccessType::Failed;
	return ActiveGameplayEffectHandle;
}
