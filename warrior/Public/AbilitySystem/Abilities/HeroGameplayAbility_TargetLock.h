// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_ExecuteOnTick.h"
#include "HeroGameplayAbility_TargetLock.generated.h"


class UInputMappingContext;
class UWarriorWidgetBase;
/**
 * 
 */
UCLASS()
class WARRIOR_API UHeroGameplayAbility_TargetLock : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
protected:
	//~Begin GameplayAbility Interface

	//能力激活时自动调用
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//能力结束时自动调用
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	//~End GameplayAbility Interface

	//在蓝图中与OnAbilityTaskTick委托绑定，而委托被每帧广播，于是此函数每帧调用。
	UFUNCTION(BlueprintCallable)
	void OnTargetLockTick(float DeltaTime);

	//实现目标切换
	UFUNCTION(BlueprintCallable)
	void SwitchTarget(const FGameplayTag& InSwitchDirectionTag);
private:
	//锁定最近的目标，如果没有目标就CancelA
	void TryLockOnTarget();
	
	//使用BoxTraceMultiForObjects进行Box碰撞判断,获得Box内所有可以锁定的目标
	void GetAvailableActorsToLock();
	
	//结束GA
	void CancelTargetLockAbility();
	
	//清空碰撞对象数组、当前锁定对象、锁定图标组件。
	void CleanUp();

	//在锁定目标状态下初始化MaxWalkSpeed,并存储默认速度
	void InitTargetLockMovement();

	//将MaxWalkSpeed重置为默认速度
	void ResetTargetLockMovement();
	
	//从所有可锁定目标中获得最近的目标
	AActor* GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors);

	//获得当前锁定目标周围可锁定对象并进行分组，根据位置分为左右两组，用于切换目标
	void GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft,TArray<AActor*>& OutActorsOnRight);

	//绘画锁定的指示组件
	void DrawTargetLockWidget();

	//把3D世界坐标转换到UI屏幕上的位置，设置锁定图标的位置
	void SetTargetLockWidgetPosition();

	//Add TargetLockMappingContext
	void InitTargetLockMappingContext();
	//Remove TargetLockMappingContext
	void ResetTargetLockMappingContext();
	
	//Box最大碰撞范围
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	float BoxTraceDistance=5000.f;
	
	//当前碰撞Box的尺寸
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	FVector TraceBoxSize=FVector(5000.f,5000.f,300.f);
	
	//碰撞对象通道，锁定敌人使用Pawn类
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	TArray<TEnumAsByte<EObjectTypeQuery>> BoxTraceChannel;
	
	//调试点
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	bool bShowPersistentDebugShape=false;

	//组件实例，因为蓝图子类UI，所以用TSubClassOf让派生类安全转换
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	TSubclassOf<UWarriorWidgetBase> TargetLockWidgetClass;

	//目标锁定时转向速度
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	float TargetLockRotationInterpSpeed=5.f;

	//锁定状态下MaxWalkSpeed
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	float TargetLockMaxWalkSpeed=150.f;
	
	//存储了覆盖输入映射的IMC
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	UInputMappingContext* TargetLockMappingContext;
	

	//锁定部件的实例对象
	UPROPERTY()
	UWarriorWidgetBase* DrawnTargetLockWidget;

	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	float TargetLockCameraOffsetDistance=20.f;
	
	//所有可锁定对象的数组
	UPROPERTY()
	TArray<AActor*> AvailableActorsToLock;
	
	//当前锁定对象
	UPROPERTY()
	AActor* CurrentLockedActor;

	//锁定图标尺寸
	UPROPERTY()
	FVector2D TargetLockWidgetSize=FVector2D::ZeroVector;

	UPROPERTY()
	float CachedDefaultMaxWalkSpeed;
	
	UPROPERTY()
	UAbilityTask_ExecuteOnTick* ExecuteOnTick;
};
