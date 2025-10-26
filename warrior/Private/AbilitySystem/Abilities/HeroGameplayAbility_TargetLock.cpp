// Yu


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/WarriorWidgetBase.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Conrtroller/WarriorHeroController.h"
#include "WarriorDebugHelper.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "Components/SizeBox.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	TryLockOnTarget();
	InitTargetLockMovement();
	InitTargetLockMappingContext();
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetTargetLockMovement();
	ResetTargetLockMappingContext();
	CleanUp();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(const float DeltaTime)
{
	//   没有锁定对象 || 有一方死亡 则解除锁定。
	if (!CurrentLockedActor || UWarriorFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor,WarriorGamePlayTags::Shared_Status_Dead) || UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(),WarriorGamePlayTags::Shared_Status_Dead))
	{
		CancelTargetLockAbility();
		return;
	}
	//每帧更新位置
	SetTargetLockWidgetPosition();

	//启动锁定GA时如果没有翻滚或格挡动作则朝向旋转
	const bool bShouldOverrideRotation=
		!UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(),WarriorGamePlayTags::Player_Status_Rolling)
	&&
		!UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(),WarriorGamePlayTags::Player_Status_Blocking);

	if (bShouldOverrideRotation)
	{
		//从起点位置看向目标位置时，在世界坐标系中应该具有的绝对旋转值。
		FRotator LookAtRot=UKismetMathLibrary::FindLookAtRotation(GetHeroCharacterFromActorInfo()->GetActorLocation(),
			CurrentLockedActor->GetActorLocation());
		//将仰角抬高CameraOffsetDistance
		LookAtRot -= FRotator(TargetLockCameraOffsetDistance,0.f,0.f);
		//获得摄像机的当前欧拉角
		const FRotator CurrentControlRot=GetHeroControllerFromActorInfo()->GetControlRotation();
		//将摄像机欧拉角加上绝对旋转值
		const FRotator TargetRot=FMath::RInterpTo(CurrentControlRot,LookAtRot,DeltaTime, TargetLockRotationInterpSpeed);
		//设置角色旋转值和摄像机旋转值为新的旋转值
		GetHeroControllerFromActorInfo()->SetControlRotation(FRotator(TargetRot.Pitch,TargetRot.Yaw,0.f));
		GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0.f,TargetRot.Yaw,0.f));
	}
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	GetAvailableActorsToLock();
	
	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetToLock=nullptr;
	//将锁定目标周围对象分组
	GetAvailableActorsAroundTarget(ActorsOnLeft,ActorsOnRight);

	//根据鼠标输入值的x轴判断后缀Tag，传入对应Tag数组
	if (InSwitchDirectionTag==WarriorGamePlayTags::Player_Event_SwitchTarget_Left)
	{
		NewTargetToLock=GetNearestTargetFromAvailableActors(ActorsOnLeft);
	}
	else
	{
		NewTargetToLock=GetNearestTargetFromAvailableActors(ActorsOnRight);
	}
	
	if (NewTargetToLock)  {CurrentLockedActor=NewTargetToLock;}
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();
	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}
	CurrentLockedActor=GetNearestTargetFromAvailableActors(AvailableActorsToLock);
	if (CurrentLockedActor)
	{
		//显现标记UI
		DrawTargetLockWidget();
		SetTargetLockWidgetPosition();
		/*Debug::Print(CurrentLockedActor->GetActorNameOrLabel());*/
	}
	else
	{
		CancelTargetLockAbility();
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	AvailableActorsToLock.Empty();
	TArray<FHitResult> BoxTraceHits;
	//从Start到End之间使用一个盒子（Box）体积做碰撞检测，检测指定类型的物体（ObjectTypes），返回所有命中的结果（MultiHit）。
	/**
	WorldContextObject	UObject*								上下文世界对象，一般传入 GetWorld() 或者 this（Actor/Ability）
	Start				FVector									盒子检测的起点位置
	End					FVector									盒子检测的终点位置（Box会从 Start 扫到 End）
	HalfSize			FVector									Box的半尺寸（X/Y/Z 各方向半宽度），所以通常传BoxExtent/2.f
	Orientation			FRotator								Box 的旋转方向，用于 Box 指向某个方向
	ObjectTypes			TArray<TEnumAsByte<EObjectTypeQuery>>	要检测的物体类型数组（比如Pawn、WorldStatic等）,UE会根据ObjectType查询碰撞
	bTraceComplex		bool									是否使用复杂碰撞（Triangle Mesh）而不是简单碰撞（Bounding Box）
	ActorsToIgnore		TArray<AActor*>							忽略列表，被忽略的 Actor 不会被检测到
	DrawDebugType		EDrawDebugTrace							调试绘制模式：None / ForDuration / Persistent 等
	OutHits				TArray<FHitResult>&						输出结果数组，每个 FHitResult 包含命中信息（位置、法线、命中 Actor 等）
	bIgnoreSelf			bool									是否忽略自身 Actor（防止自己被检测到）**/
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		GetHeroCharacterFromActorInfo()->GetActorLocation()+GetHeroCharacterFromActorInfo()->GetActorForwardVector()*BoxTraceDistance,
		TraceBoxSize/2.f,
		GetHeroCharacterFromActorInfo()->GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannel,
		false,
		TArray<AActor*>(),
		bShowPersistentDebugShape?EDrawDebugTrace::Persistent:EDrawDebugTrace::None,
		BoxTraceHits,
		true);
	//存储了所有BoxSize的碰撞Result
	for (const FHitResult& TraceHit : BoxTraceHits)
	{
		//获得Result的碰撞对象
		if (AActor* HitActor = TraceHit.GetActor())
		{
			//不能是自己
			if (HitActor!= GetHeroCharacterFromActorInfo())
			{
				//将所有碰撞到的Pawn都添加到AvailableActorsToLock数组
				AvailableActorsToLock.AddUnique(HitActor);
				/*Debug::Print(HitActor->GetActorNameOrLabel());	*/
			}
		}
	}
}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	//ActivationInfo()为当前Ability激活时的上下文信息，与spec的context不同，它是临时存在的，存储的是激活模式、预测Key、Source对象等
	//而Spec是Cooldown、Cost、Level、Context等配置数据
	CancelAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true);
}

void UHeroGameplayAbility_TargetLock::CleanUp()
{
	
	AvailableActorsToLock.Empty();
	CurrentLockedActor=nullptr;
	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}
	DrawnTargetLockWidget=nullptr;
	TargetLockWidgetSize=FVector2D::ZeroVector;
	CachedDefaultMaxWalkSpeed=0.f;
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	CachedDefaultMaxWalkSpeed=GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed;
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed=TargetLockMaxWalkSpeed;
}
void UHeroGameplayAbility_TargetLock::ResetTargetLockMovement()
{
	if (CachedDefaultMaxWalkSpeed>0.f)
	{
		GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed=CachedDefaultMaxWalkSpeed;
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	//只做参数，不需要知道具体距离，所以使用局部变量
	float ClosestDistance=0.f;
	//找到最近的Actor
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(),InAvailableActors,ClosestDistance);
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft,
	TArray<AActor*>& OutActorsOnRight)
{
	if (!CurrentLockedActor || AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}
	const FVector PlayerLocation=GetHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentNormalized=(CurrentLockedActor->GetActorLocation()-PlayerLocation).GetSafeNormal();

	for (AActor* AvailableActor : AvailableActorsToLock)
	{
		if (!AvailableActor || AvailableActor==CurrentLockedActor) continue;
		const FVector PlayerToAvailableNormalized=(AvailableActor->GetActorLocation()-PlayerLocation).GetSafeNormal();
		//归一化
		const FVector CrossResult=FVector::CrossProduct(PlayerToCurrentNormalized,PlayerToAvailableNormalized);
		//向量数学叉积概念，当z轴在上，则B在A的右边，否则B在A的左边
		if (CrossResult.Z>0.f)
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}
		else
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
}

void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass,TEXT("Forgot to assign a valid widget class in blueprint"));
	
		DrawnTargetLockWidget=CreateWidget<UWarriorWidgetBase>(GetHeroControllerFromActorInfo(),TargetLockWidgetClass);
		check(DrawnTargetLockWidget)
		//Adds it to the game's viewport and fills the entire screen，需要手动调整其大小
		DrawnTargetLockWidget->AddToViewport();
	}

}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawnTargetLockWidget || !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}
	//一个2DVctor坐标
	FVector2D ScreenPos;
	/**把3D世界坐标投影成2D屏幕坐标**/
	
	//GetHeroControllerFromActorInfo()用来获取当前视口信息（摄像机矩阵）
	//若为true，坐标相对于玩家视口；若为 false，为全屏绝对坐标
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetHeroControllerFromActorInfo(),CurrentLockedActor->GetActorLocation(),ScreenPos,true);
	//判断TargetLockWidgetSize是否为0，如果是，说明坐标还没有设置
	if(TargetLockWidgetSize==FVector2D::ZeroVector)
		{
		//找到Widget树中的USizeBox,ForEachWidget()是UWidgetTree的成员函数，用于遍历当前Widget树中所有的子Widget。
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
			[this](UWidget* FoundWidget)
			{
				//对于每一个找到的子Widget尝试进行类型转换，如果成功说明找到了树中的USizeBox
				//??这样做的理由：Widget的遍历开销很小，这样找很方便   优化：可以直接缓存引用，然后这里直接使用
				if (USizeBox* FoundSizeBox=Cast<USizeBox>(FoundWidget))
				{
					//获得图标尺寸
					TargetLockWidgetSize.X=FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y=FoundSizeBox->GetHeightOverride();
				}
			});
	}
	//由于X、Y是图标长度的最大值，即指向了左上角（在FVector2D中y轴向下是加，x轴向右是加）
	ScreenPos-=(TargetLockWidgetSize/2.f);
	//将图标左上角对齐ScreenPos，而我们希望居中，所以传入的值ScreenPos向左上对应移动一半的位置（将图标整体向左上拉）
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPos,false);
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	//通过Controller获取LocalPlayer，通过它获取UEnhancedInputLocalPlayerSubsystem类SubSystem
	const ULocalPlayer* LocalPlayer=GetHeroControllerFromActorInfo()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(Subsystem);
	//优先级应该高于武器的MappingContext，因为要覆盖其2D鼠标输入
	Subsystem->AddMappingContext(TargetLockMappingContext,3);
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	//这个判断的意义在于当我们激活能力状态下退出游戏时，会调用EndAbility，但游戏退出时已经没有了HeroController，这将导致程序崩溃
	if (!GetHeroControllerFromActorInfo())
	{
		return ;
	}
	const ULocalPlayer* LocalPlayer=GetHeroControllerFromActorInfo()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(Subsystem);

	Subsystem->RemoveMappingContext(TargetLockMappingContext);
}
