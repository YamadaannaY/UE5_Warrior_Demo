// Yu


#include "Characters/WarriorBaseCharacter.h"
#include "MotionWarpingComponent.h"


AWarriorBaseCharacter::AWarriorBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled=false;
	
	//Mesh 是否能接收贴花（Decal）渲染，关闭可减少开销，默认开启
	GetMesh()->bReceivesDecals=false;

	WarriorAbilitySystemComponent=CreateDefaultSubobject<UWarriorAbilitySystemComponent>(TEXT("WarriorAbilitySystemComponent"));
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
		//必须初始化让ASC知道控制的角色是谁，Owner = 谁拥有这个能力 // Avatar = 谁执行这个能力，通常都是this，这里决定了后续两个值的指向
		WarriorAbilitySystemComponent->InitAbilityActorInfo(this,this);
		
		//如果CharacterStartUpData不存在，此处成为断点并在Log中返回信息
		ensureMsgf(! CharacterStartUpData.IsNull(),TEXT("Forgot to assign strat up data to %s"),*GetName());
	}
}


