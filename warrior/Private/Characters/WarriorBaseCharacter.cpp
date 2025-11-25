// Yu


#include "Characters/WarriorBaseCharacter.h"
#include "MotionWarpingComponent.h"
#include "WarriorDebugHelper.h"


AWarriorBaseCharacter::AWarriorBaseCharacter()
{
	//配置逻辑执行一次即可
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled=false;

	//ASC
	WarriorAbilitySystemComponent=CreateDefaultSubobject<UWarriorAbilitySystemComponent>(TEXT("WarriorAbilitySystemComponent"));
	WarriorAbilitySystemComponent->SetIsReplicated(true);
	WarriorAbilitySystemComponent->SetReplicationMode(AscReplicationMode);

	WarriorAttributeSet=CreateDefaultSubobject<UWarriorAttributeSet>(TEXT("WarriorAttributeSet"));

	MotionWarpingComponent=CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}


UAbilitySystemComponent* AWarriorBaseCharacter::GetAbilitySystemComponent() const
{
	return GetWarriorAbilitySystemComponent();
}

UPawnCombatComponent* AWarriorBaseCharacter::GetPawnCombatComponent() const
{
	//子类复写用于为接口所调用
	return nullptr;
}

UPawnUIComponent* AWarriorBaseCharacter::GetPawnUIComponent() const
{
	//子类复写用于为接口所调用
	return nullptr;
}

//当角色被ControllerPossess时调用，GAS的核心：每个拥有能力的Actor都要初始化 ASC（Ability System Component）的AbilityActorInfo，
//往往在PossessedBy中进行
void AWarriorBaseCharacter:: PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (WarriorAbilitySystemComponent)
	{
		//使用ASC必须在PossessedBy初始化ActorInfo，Owner = 谁拥有这个能力 // Avatar = 谁执行这个能力，通常都是this，这里决定了后续两个值的指向
		WarriorAbilitySystemComponent->InitAbilityActorInfo(this,this);
		
		if(CharacterStartUpData.IsNull())
		{
			Debug::Print(FString::Printf(TEXT("Forgot to assign strat up data to %s"),*GetName()),FColor::Red);
		}
	}
}


