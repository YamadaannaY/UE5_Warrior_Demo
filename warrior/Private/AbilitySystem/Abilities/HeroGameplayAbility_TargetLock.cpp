// Yu


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"
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



void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	TryLockOnTarget();
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	CleanUp();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	//没有锁定对象 || 有一方死亡 则解除锁定。
	if (!CurrentLockedActor || UWarriorFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedActor,WarriorGamePlayTags::Shared_Status_Dead) || UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetHeroCharacterFromActorInfo(),WarriorGamePlayTags::Shared_Status_Dead))
	{
		CancelTargetLockAbility();
		return;
	}
	//每帧更新位置
	SetTargetLockWidgetPosition();
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
	//清空碰撞对象数组、当前锁定对象、锁定图标组件。
	AvailableActorsToLock.Empty();
	CurrentLockedActor=nullptr;
	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}
	DrawnTargetLockWidget=nullptr;
	TargetLockWidgetSize=FVector2D::ZeroVector;
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	//只做参数，不需要知道具体距离，所以使用局部变量
	float ClosestDistance=0.f;
	//找到最近的Actor
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(),InAvailableActors,ClosestDistance);
}

void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass,TEXT("Forgot to assign a valid widget class in blueprint"));
	
		DrawnTargetLockWidget=CreateWidget<UWarriorWidgetBase>(GetHeroControllerFromActorInfo(),TargetLockWidgetClass);
		check(DrawnTargetLockWidget)
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
	FVector2D ScreenPos;
	//把3D世界坐标投影成2D屏幕坐标
	//GetHeroControllerFromActorInfo()用来获取当前视口信息（摄像机矩阵）
	//若为true，坐标相对于玩家视口；若为 false，为全屏绝对坐标
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetHeroControllerFromActorInfo(),CurrentLockedActor->GetActorLocation(),ScreenPos,true);
	//判断TargetLockWidgetSize是否为0
	if(TargetLockWidgetSize==FVector2D::ZeroVector)
		{
		//找到Widget树中的USizeBox,ForEachWidget()是UWidgetTree的成员函数，用于遍历当前Widget树中所有的子Widget。
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
			[this](UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox=Cast<USizeBox>(FoundWidget))
				{
					//获得图标尺寸
					TargetLockWidgetSize.X=FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y=FoundSizeBox->GetHeightOverride();
				}
			});
	}
	//由于X、Y是图标长度的最大值，即指向了左上角，
	ScreenPos-=(TargetLockWidgetSize/2.f);
	//将图标左上角对齐ScreenPos，而我们希望居中，所以传入的值ScreenPos向左上对应移动一半的位置（将图标向左上拉）
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPos,false);
}
