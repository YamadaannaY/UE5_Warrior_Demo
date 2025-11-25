// Yu


#include "Characters/WarriorEnemyCharacter.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DataAssets/StartUpData/DataAsset_EnemyStartUpData.h"
#include "Components/UI/EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "Widgets/WarriorWidgetBase.h"
#include "Components/BoxComponent.h"
#include "Engine/AssetManager.h"
#include "WarriorFunctionLibrary.h"
#include "GameMode/WarriorGameMode.h"


AWarriorEnemyCharacter::AWarriorEnemyCharacter()
{
	//不随控制旋转而改变，依靠行为树实现旋转
	bUseControllerRotationPitch=false;
	bUseControllerRotationYaw=false;
	bUseControllerRotationRoll=false;
	
	EnemyCombatComponent=CreateDefaultSubobject<UEnemyCombatComponent>(TEXT("EnemyCombatComponent"));
	EnemyUIComponent=CreateDefaultSubobject<UEnemyUIComponent>(TEXT("EnemyUIComponent"));
	EnemyHealthWidgetComponent=CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHealthWidgetComponent"));

	//将生命值条与Mesh绑定
	EnemyHealthWidgetComponent->SetupAttachment(GetMesh());

	//将Box与Mesh绑定，初始化为无碰撞，并将OnComponentBeginOverlap绑定回调
	LeftHandCollisionBox=CreateDefaultSubobject<UBoxComponent>("LeftHandCollisionBox");
	LeftHandCollisionBox->SetupAttachment(GetMesh());
	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	LeftHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnBodyCollisionBoxBeginOverlap);
	
	RightHandCollisionBox=CreateDefaultSubobject<UBoxComponent>("RightHandCollisionBox");
	RightHandCollisionBox->SetupAttachment(GetMesh());
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	RightHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnBodyCollisionBoxBeginOverlap);
	
	//面向移动方向旋转，同时不依赖Controller
	GetCharacterMovement()->bOrientRotationToMovement=true;
	GetCharacterMovement()->bUseControllerDesiredRotation=false;
	GetCharacterMovement()->RotationRate=FRotator(0.0f,540.0f,0.0f);
	GetCharacterMovement()->MaxWalkSpeed=400.f;
	GetCharacterMovement()->BrakingDecelerationWalking=1000.f;
}

UPawnCombatComponent* AWarriorEnemyCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

UPawnUIComponent* AWarriorEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

UEnemyUIComponent* AWarriorEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

#if WITH_EDITOR
void AWarriorEnemyCharacter::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	// 当LeftHandCollisionBoxAttachBoneName属性变化时，立即将左手Box附着到新的骨骼
	if (PropertyChangedEvent.GetMemberPropertyName()==GET_MEMBER_NAME_CHECKED(ThisClass,LeftHandCollisionBoxAttachBoneName))
	{
		LeftHandCollisionBox->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,LeftHandCollisionBoxAttachBoneName);
	}
	// 当 RightHandCollisionBoxAttachBoneName属性变化时，立即将右手Box附着到新的骨骼
	if (PropertyChangedEvent.GetMemberPropertyName()==GET_MEMBER_NAME_CHECKED(ThisClass,RightHandCollisionBoxAttachBoneName))
	{
		RightHandCollisionBox->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,RightHandCollisionBoxAttachBoneName);
	}
}
#endif

void AWarriorEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	//获得组件管理的实例以操作这个实例
	if (UWarriorWidgetBase* HealthWidget=Cast<UWarriorWidgetBase>(EnemyHealthWidgetComponent->GetUserWidgetObject()))
	{
		//只在Enemy产生时对应产生Widget而非引擎初始化时创建，节省开销。
		HealthWidget->InitEnemyCreatedWidget(this);
	}
}

void AWarriorEnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (APawn* HitPawn=Cast<APawn>(OtherActor))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(this,HitPawn))
		{
			EnemyCombatComponent->OnHitTargetActor(HitPawn);
		}
	}
}

void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//Possess即AIC加载，此时开始加载异步数据
	InitEnemyStartUpData();
}

void AWarriorEnemyCharacter::InitEnemyStartUpData() const
{
	if (CharacterStartUpData.IsNull())
	{
		return ;
	}

	int32 AbilityApplyLevel=1;
			
	if (const AWarriorGameMode* BaseGameMode=GetWorld()->GetAuthGameMode<AWarriorGameMode>())
	{
		switch (BaseGameMode->GetCurrentGameDifficulty())
		{
		case EWarriorGameDifficulty::Easy:
			AbilityApplyLevel=1;
			break;
		case EWarriorGameDifficulty::Normal:
			AbilityApplyLevel=2;
			break;
		case EWarriorGameDifficulty::Hard:
			AbilityApplyLevel=3;
			break;
		case EWarriorGameDifficulty::VeryHard:
			AbilityApplyLevel=4;
			break;
		default:
			break;
		}
	}
	
	if (UDataAsset_StartUpDataBase* LoadedData=CharacterStartUpData.LoadSynchronous())
	{
		//异步加载固定流程
		//CreateLambda用于无参调用函数
		UAssetManager::GetStreamableManager().RequestAsyncLoad(CharacterStartUpData.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda(
				[this,AbilityApplyLevel]()
				{
					if (UDataAsset_StartUpDataBase* LoadedData=CharacterStartUpData.Get())
					{
						//加载成功将其所有GASpec赋予ASC
						LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent,AbilityApplyLevel);
					}
				}
			)
		);
	}
}


