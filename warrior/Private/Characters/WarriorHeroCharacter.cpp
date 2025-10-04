// Yu


#include "Characters/WarriorHeroCharacter.h"
#include  "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Components/Input/WarriorInputComponent.h"
#include  "WarriorGamePlayTags.h"
#include "WarriorDebugHelper.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "Components/Combat/HeroCombatComponent.h"


void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//若起始数据不为空
	if (!CharacterStartUpData.IsNull())
	{
		//如果软引用同步加载并赋值成功
		if (UDataAsset_StartUpDataBase* LoadedData=CharacterStartUpData.LoadSynchronous())
		{
			//将StartupData中存储的能力赋予ASC
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
		}
	}
	//将属性集添加到ASC中
	WarriorAbilitySystemComponent->AddAttributeSetSubobject(WarriorAttributeSet);
}

void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AWarriorHeroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset,TEXT("Forgot to assign a valid data asset as input config"))
	//单机中local player指的是被Controller控制的pawn
	ULocalPlayer* LocalPlayer=GetController<APlayerController>()->GetLocalPlayer();
	
	//获得可供LocalPlayer使用的EnhancedInputSubsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(Subsystem);
	
	//传入mapping context以存储所有的动作映射
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext,0);
	
	//if false,return nullptr
	UWarriorInputComponent* WarriorInputComponent=CastChecked<UWarriorInputComponent>(PlayerInputComponent);

	/**将具体的函数打上tag标签并说明触发方式**/
	
	//Native
	WarriorInputComponent->BindNativeInputAction(
		InputConfigDataAsset,
		WarriorGamePlayTags::InputTag_Move,
		ETriggerEvent::Triggered,
		this,&ThisClass::Input_Move);
	WarriorInputComponent->BindNativeInputAction(
		InputConfigDataAsset,
		WarriorGamePlayTags::InputTag_Look,
		ETriggerEvent::Triggered,
		this,&ThisClass::Input_Look);
	//Ability
	WarriorInputComponent->BindAbilityInputAction(
		InputConfigDataAsset,
		this,
		&ThisClass::Input_AbilityInputPressed,
		&ThisClass::Input_AbilityInputReleased);
}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector=InputActionValue.Get<FVector2D>();
	//获得摄像机的朝向，并将其作为前进方向的旋转矢量，使得前进矢量修改为摄像机朝向的向前矢量
	const FRotator MovementRotation(0.f,Controller->GetControlRotation().Yaw,0.f);
	if (MovementVector.Y!=0.f)
	{
		const FVector ForwardDirection=MovementRotation.RotateVector(FVector::ForwardVector);
		//通过方向与方向矢量上值大小的加权获得Add
		AddMovementInput(ForwardDirection,MovementVector.Y);
		
	}
	if (MovementVector.X!=0.f)
	{
		//将摄像机朝向作为旋转矢量，使得X方向上矢量为摄像机朝向的左右方向矢量
		const FVector RightDirection=MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection,MovementVector.X);
	}
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector=InputActionValue.Get<FVector2D>();
	if (LookAxisVector.X!=0.f)
	{
		//根据鼠标controller获得x、y的分量，根据x y增加controller的水平分量，而相机臂设置了跟随controller旋转，
		//相机跟随相机臂，所以会获得输入。
		//CameraBoom->bUsePawnControlRotation=true;
		AddControllerYawInput(LookAxisVector.X);
	}
	if (LookAxisVector.Y!=0.f)
	{
		AddControllerPitchInput(-LookAxisVector.Y); 
	}
}

void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}


AWarriorHeroCharacter::AWarriorHeroCharacter()
{
	//set Capsule 大小
	GetCapsuleComponent()->InitCapsuleSize(42.f,96.f);
	//角色不跟随控制器（本游戏中是鼠标）旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	CameraBoom=CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	//ArmLength
	CameraBoom->TargetArmLength=200.f;
	//SocketOffset：相机臂尾部与连接的组件的距离
	CameraBoom->SocketOffset=FVector(0.f,55.f,65.f);
	//让角色控制器来带动相机臂旋转
	CameraBoom->bUsePawnControlRotation=true;
	
	FollowCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	//相机臂已设置，不要重复，否则旋转叠加产生风险
	FollowCamera->bUsePawnControlRotation=false;

	//设置角色朝向运动方向
	GetCharacterMovement()->bOrientRotationToMovement=true;
	//设置朝向旋转速度
	GetCharacterMovement()->RotationRate=FRotator(0.0f,500.0f,0.0f);
	//设置速度
	GetCharacterMovement()->MaxWalkSpeed=500.f;
	//设置加速度
	GetCharacterMovement()->BrakingDecelerationWalking=2000.f;
	
	HeroCombatComponent=CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
	HeroUIComponent=CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
}

UPawnCombatComponent* AWarriorHeroCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

UPawnUIComponent* AWarriorHeroCharacter::GetPawnUIComponent() const
{
	return HeroUIComponent;
}

UHeroUIComponent* AWarriorHeroCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}


