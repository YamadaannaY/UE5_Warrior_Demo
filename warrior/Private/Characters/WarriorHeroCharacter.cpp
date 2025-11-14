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
#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorDebugHelper.h"
#include "Components/UI/HeroUIComponent.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "GameMode/WarriorGameMode.h"


void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//若起始数据不为空
	if (!CharacterStartUpData.IsNull())
	{
		//由于Player一开始就被Controller Possess，且十分重要，所以进行同步加载
		if (UDataAsset_StartUpDataBase* LoadedData=CharacterStartUpData.LoadSynchronous())
		{
			int32 AbilityApplyLevel=1;
			
			if (const AWarriorGameMode* BaseGameMode=GetWorld()->GetAuthGameMode<AWarriorGameMode>())
			{
				switch (BaseGameMode->GetCurrentGameDifficulty())
				{
				case EWarriorGameDifficulty::Easy:
					AbilityApplyLevel=4;
					/*Debug::Print(TEXT("easy"));*/
					break;
				case EWarriorGameDifficulty::Normal:
					AbilityApplyLevel=3;
					/*Debug::Print(TEXT("normal"));*/
					break;
				case EWarriorGameDifficulty::Hard:
					AbilityApplyLevel=2;
					/*Debug::Print(TEXT("hard"));*/
					break;
				case EWarriorGameDifficulty::VeryHard:
					AbilityApplyLevel=1;
					/*Debug::Print(TEXT("very hard"));*/
					break;
				default:
					break;
				}
			}
			//将DA赋予ASC
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent,AbilityApplyLevel);
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
	//Local player指的是被本地Controller控制的pawn
	const ULocalPlayer* LocalPlayer=GetController<APlayerController>()->GetLocalPlayer();
	
	//获得LocalPlayer使用的EnhancedInputSubsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(Subsystem);
	
	//传入IMC以存储所有的动作映射，默认IMC优先级最低
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext,0);
	
	UWarriorInputComponent* WarriorInputComponent=CastChecked<UWarriorInputComponent>(PlayerInputComponent);
	
	//Native
	WarriorInputComponent->BindNativeInputAction(
		InputConfigDataAsset,
		WarriorGamePlayTags::InputTag_Move,
		ETriggerEvent::Triggered,this,&ThisClass::Input_Move);
	
	WarriorInputComponent->BindNativeInputAction
	(InputConfigDataAsset,WarriorGamePlayTags::InputTag_Look,
		ETriggerEvent::Triggered,this,&ThisClass::Input_Look);
	
	WarriorInputComponent->BindNativeInputAction
	(InputConfigDataAsset,WarriorGamePlayTags::InputTag_SwitchTarget,
		ETriggerEvent::Triggered,this,&ThisClass::Input_SwitchTargetTriggered);
	WarriorInputComponent->BindNativeInputAction

	(InputConfigDataAsset,WarriorGamePlayTags::InputTag_SwitchTarget,
		ETriggerEvent::Completed,this,&ThisClass::Input_SwitchTargetCompleted);
	
	WarriorInputComponent->BindNativeInputAction
	(InputConfigDataAsset,WarriorGamePlayTags::InputTag_PickUp_Stone,
		ETriggerEvent::Started,this,&ThisClass::Input_PickUp_Stone_Started);
	
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
	//获得摄像机水平方向的欧拉角旋转
	const FRotator MovementRotation(0.f,Controller->GetControlRotation().Yaw,0.f);
	
	if (MovementVector.Y!=0.f)
	{
		 //旋转到的欧拉角度单位向量
		const FVector ForwardDirection=MovementRotation.RotateVector(FVector::ForwardVector);
		//ForwardDirection * MovementVector.Y
		AddMovementInput(ForwardDirection,MovementVector.Y);
	}
	if (MovementVector.X!=0.f)
	{
		//X方向也有输入值，获得摄像机当前左右方向单位向量。
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
		//使得Y映射向下时摄像机向上，符合直觉
		AddControllerPitchInput(-LookAxisVector.Y); 
	}
}

void AWarriorHeroCharacter::Input_SwitchTargetTriggered(const FInputActionValue& InputActionValue)
{
	SwitchDirection=InputActionValue.Get<FVector2D>();
}

void AWarriorHeroCharacter::Input_SwitchTargetCompleted(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;
	//水平方向大于0，即向右移动鼠标，则激活右切换，否则是左切换
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,SwitchDirection.X>0.f ? WarriorGamePlayTags::Player_Event_SwitchTarget_Right : WarriorGamePlayTags::Player_Event_SwitchTarget_Left,
		Data);
	Debug::Print(TEXT("Switch Direction:")+SwitchDirection.ToString());
}

void AWarriorHeroCharacter::Input_PickUp_Stone_Started(const FInputActionValue& InputActionValue)
{
	FGameplayEventData Data;
	//输入映射会触发WaitGamePlayEvent，将Stone的效果GE赋予Player
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,WarriorGamePlayTags::Player_Event_ConsumeStones,Data);
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
	//角色不跟随控制器（鼠标）旋转
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
	//相机臂已设置旋转，不要重复，否则旋转叠加产生风险
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


