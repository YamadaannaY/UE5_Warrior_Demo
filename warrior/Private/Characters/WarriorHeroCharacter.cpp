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
#include "MotionWarpingComponent.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "Components/UI/HeroUIComponent.h"
#include "DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "GameMode/WarriorGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetMathLibrary.inl"

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
	CameraBoom->TargetArmLength=200.f;
	CameraBoom->SocketOffset=FVector(0.f,55.f,65.f);
	CameraBoom->bUsePawnControlRotation=true;
	
	FollowCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation=false;

	//设置角色朝向运动方向
	GetCharacterMovement()->bOrientRotationToMovement=true;
	//设置朝向旋转速度
	GetCharacterMovement()->RotationRate=FRotator(0.0f,720.f,0.0f);
	//设置速度
	GetCharacterMovement()->MaxWalkSpeed=500.f;
	//设置加速度
	GetCharacterMovement()->BrakingDecelerationWalking=2000.f;
	
	HeroCombatComponent=CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
	HeroUIComponent=CreateDefaultSubobject<UHeroUIComponent>(TEXT("HeroUIComponent"));
}


void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	//起始数据不为空
	if (!CharacterStartUpData.IsNull())
	{
		if (UDataAsset_StartUpDataBase* LoadedData=CharacterStartUpData.LoadSynchronous())
		{
			AbilityApplyLevel=1;
			
			if (const AWarriorGameMode* BaseGameMode=GetWorld()->GetAuthGameMode<AWarriorGameMode>())
			{
				switch (BaseGameMode->GetCurrentGameDifficulty())
				{
				case EWarriorGameDifficulty::Easy:
					AbilityApplyLevel=4;
					break;
				case EWarriorGameDifficulty::Normal:
					AbilityApplyLevel=3;
					break;
				case EWarriorGameDifficulty::Hard:
					AbilityApplyLevel=2;
					break;
				case EWarriorGameDifficulty::VeryHard:
					AbilityApplyLevel=1;
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
	
	//延时0.1s加载ASC
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::InitializeAttributeListener, 0.1f, false);
}


void AWarriorHeroCharacter::InitializeAttributeListener()
{
	if (!GetWarriorAbilitySystemComponent() || !GetWarriorAttributeSet())
	{
		return;
	}
	
	GetWarriorAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
		UWarriorAttributeSet::GetStaminaAttribute()
	).AddUObject(this, &AWarriorHeroCharacter::HandleStaminaChangeDirect);
	
	GetWarriorAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
		UWarriorAttributeSet::GetCurrentHealthAttribute()
	).AddUObject(this, &AWarriorHeroCharacter::HandleCurrentHealthChangeDirect);
	
	GetWarriorAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
		UWarriorAttributeSet::GetCurrentRageAttribute()
	).AddUObject(this, &AWarriorHeroCharacter::HandleCurrentRageDirect);
}

void AWarriorHeroCharacter::HandleStaminaChangeDirect(const FOnAttributeChangeData& Data) const 
{
	const float NewValue = Data.NewValue;
	const float OldValue = Data.OldValue;

	//处理Stamina逻辑
	HandleStaminaChange(UWarriorAttributeSet::GetStaminaAttribute(),NewValue, OldValue);
}

void AWarriorHeroCharacter::HandleCurrentHealthChangeDirect(const FOnAttributeChangeData& Data)
{
	const float NewValue = Data.NewValue;
	const float OldValue = Data.OldValue;
	if (NewValue<=0.f)
	{
		UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(this,WarriorGamePlayTags::Shared_Status_Dead);
		return;
	}
	//处理CurrentHealth
	HandleCurrentHealthChange(UWarriorAttributeSet::GetCurrentHealthAttribute(),NewValue,OldValue);
}

void AWarriorHeroCharacter::HandleCurrentRageDirect(const FOnAttributeChangeData& Data) const 
{
	const float NewValue = Data.NewValue;
	const float OldValue = Data.OldValue;

	//处理CurrentRage
	HandleCurrentRageChange(UWarriorAttributeSet::GetCurrentRageAttribute(),NewValue, OldValue);
}

void AWarriorHeroCharacter::Multicast_SetRollData_Implementation(FVector InDirection,FVector InTarget)
{
	RollDirection = InDirection;
	RollTargetLocation = InTarget;
	ApplyWarpTargets();
}

/*------------------------------------------
			 Apply Warp Targets
  (Called on client + server after replication)
--------------------------------------------*/
void AWarriorHeroCharacter::ApplyWarpTargets()
{
	if (!MotionWarpingComponent) return;

	FRotator RollRotation=UKismetMathLibrary::MakeRotFromX(RollDirection);
	// 设置方向
	if (!RollDirection.IsZero())
	{
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName("RollingDirection"),FVector::ZeroVector,RollRotation);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("RollDirection Not Found"))
	}
	// 设置落点
	if (!RollTargetLocation.IsZero())
	{
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(FName("RollTargetLocation"),RollTargetLocation);
	}
	else
	{
		MotionWarpingComponent->RemoveWarpTarget(FName("RollTargetLocation"));
	}
}

void AWarriorHeroCharacter::HandleStaminaChange(FGameplayAttribute ChangeAttribute,float NewValue, float OldValue) const 
{
	if (NewValue < OldValue)
	{
		const FGameplayEffectSpecHandle SpecHandle =
			GetWarriorAbilitySystemComponent()->MakeOutgoingSpec
		(
			StaminaRegenEffect, 
		    AbilityApplyLevel, 
		    GetWarriorAbilitySystemComponent()->MakeEffectContext()
		);

		if (SpecHandle.IsValid())
		{
			//每次Dash之后延时一秒再进行StaminaRegen，所以无论GE是否存在都要删除，否则除第一次之外不会触发延时
			GetWarriorAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(StaminaRegenEffectTagToRemove.GetSingleTagContainer());

			//设置一个定时器，在判断栈代码之后1s后触发Apply
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [this,SpecHandle]()
			{
				
				GetWarriorAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
			},1.0f,false); // 1秒，不循环
		}
	}
	else
	{
		if(NewValue >= GetWarriorAttributeSet()->MaxStamina.GetCurrentValue())
		{
			GetWarriorAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(StaminaRegenEffectTagToRemove.GetSingleTagContainer());
		}
	}
}

void AWarriorHeroCharacter::HandleCurrentHealthChange(FGameplayAttribute Attribute, float NewValue,
	float OldValue) const
{
	if (NewValue < OldValue)
	{
		const FGameplayEffectSpecHandle SpecHandle =
			GetWarriorAbilitySystemComponent()->MakeOutgoingSpec
		(
			CurrentHealthEffect, 
			AbilityApplyLevel, 
			GetWarriorAbilitySystemComponent()->MakeEffectContext()
		);

		if (SpecHandle.IsValid())
		{
			//每次Dash之后延时一秒再进行StaminaRegen，所以无论GE是否存在都要删除，否则除第一次之外不会触发延时
			GetWarriorAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(CurrentHealthEffectTagToRemove.GetSingleTagContainer());

			//设置一个定时器，在判断栈代码之后1s后触发Apply
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [this,SpecHandle]()
			{
				
				GetWarriorAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
			},1.0f,false);
		}
	}
	else
	{
		if(NewValue >= GetWarriorAttributeSet()->MaxHealth.GetCurrentValue())
		{
			GetWarriorAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(CurrentHealthEffectTagToRemove.GetSingleTagContainer());
		}
	}
}

void AWarriorHeroCharacter::HandleCurrentRageChange(FGameplayAttribute Attribute, float NewValue, float OldValue) const
{
	if (NewValue < OldValue)
	{
		const FGameplayEffectSpecHandle SpecHandle =
			GetWarriorAbilitySystemComponent()->MakeOutgoingSpec
		(
			CurrentRageEffect, 
			AbilityApplyLevel, 
			GetWarriorAbilitySystemComponent()->MakeEffectContext()
		);

		if (SpecHandle.IsValid())
		{
			GetWarriorAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(CurrentRageEffectTagToRemove.GetSingleTagContainer());

			//设置一个定时器，在判断栈代码之后1s后触发Apply
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [this,SpecHandle]()
			{
				GetWarriorAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
			},1.0f,false);
		}
	}
	else
	{
		if(NewValue >= GetWarriorAttributeSet()->MaxRage.GetCurrentValue())
		{
			GetWarriorAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(CurrentRageEffectTagToRemove.GetSingleTagContainer());
		}
	}
}

void AWarriorHeroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if(!InputConfigDataAsset){Debug::Print(TEXT("Forgot to assign a valid data asset as input config"));}

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
	WarriorInputComponent->BindAbilityInputAction(InputConfigDataAsset,this,&ThisClass::Input_AbilityInputPressed,&ThisClass::Input_AbilityInputReleased);
}
#pragma region Input_Action

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	//W+D 组合输入时会是 (1,1)，向量长度 √2,也就是输入值比直线更快，通过归一化获取单位向量进行避免
	FVector2D InputVal=InputActionValue.Get<FVector2d>();
	InputVal.Normalize();

	//将摄像机的坐标系作为位移方向参考，添加映射实现位移
	AddMovementInput(GetMoveFwdDir()*InputVal.Y+GetLookRightDir()*InputVal.X);
}

FVector AWarriorHeroCharacter::GetLookRightDir() const
{
	return FollowCamera->GetRightVector();
}

FVector AWarriorHeroCharacter::GetLookFwdDir() const
{
	return FollowCamera->GetForwardVector();
}

FVector AWarriorHeroCharacter::GetMoveFwdDir() const
{
	//左手定则,以大拇指为A，食指为B，中指即结果Vector
	return FVector::CrossProduct(GetLookRightDir(),FVector::UpVector);
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
	//输入映射会触发WaitGamePlayEvent，将Stone的效果GE赋予Player
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,WarriorGamePlayTags::Player_Event_ConsumeStones,FGameplayEventData());
}

void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}
#pragma endregion Input_Action


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