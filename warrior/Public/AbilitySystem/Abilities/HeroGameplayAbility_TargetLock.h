// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
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
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	//~End GameplayAbility Interface

	//这个函数在蓝图中与OnAbilityTaskTick委托连接，而委托被每帧广播，于是此函数每帧调用。
	UFUNCTION(BlueprintCallable)
	void OnTargetLockTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void SwitchTarget(const FGameplayTag& InSwitchDirectionTag);
private:
	//锁定最近的目标，如果没有目标就Cancel锁定能力
	void TryLockOnTarget();
	
	//使用BoxTraceMultiForObjects进行Box碰撞判断获得所有可以锁定的目标
	void GetAvailableActorsToLock();
	
	//接口函数，在其他函数判断中用于结束能力
	void CancelTargetLockAbility();
	
	//清空所有目标，将最近目标的指针设为空
	void CleanUp();

	//在锁定目标状态下初始化MaxWalkSpeed,并缓存默认速度
	void InitTargetLockMovement();

	//将MaxWalkSpeed重置为缓存的默认速度
	void ResetTargetLockMovement();
	
	//使用（UGameplayStatics::FindNearestActor）从所有可锁定目标中获得最近的目标
	AActor* GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors);

	void GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft,TArray<AActor*>& OutActorsOnRight);

	//绘画锁定的指示组件
	void DrawTargetLockWidget();

	//把3D世界坐标转换到UI屏幕上的位置，设置锁定图标的位置
	void SetTargetLockWidgetPosition();

	void InitTargetLockMappingContext();

	void ResetTargetLockMappingContext();
	
	//可碰撞范围
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	float BoxTraceDistance=5000.f;
	
	//碰撞Box的尺寸
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	FVector TraceBoxSize=FVector(5000.f,5000.f,300.f);
	
	//碰撞对象的类型，锁定敌人使用Pawn
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	TArray<TEnumAsByte<EObjectTypeQuery>> BoxTraceChannel;
	
	//调试点
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	bool bShowPersistentDebugShape=false;

	//组件对象，因为用到的是蓝图子类，所以用TSubClassOf
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	TSubclassOf<UWarriorWidgetBase> TargetLockWidgetClass;

	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	float TargetLockRotationInterpSpeed=5.f;

	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	float TargetLockMaxWalkSpeed=150.f;
	
	UPROPERTY(EditDefaultsOnly,Category="TargetLock")
	UInputMappingContext* TargetLockMappingContext;
	

	//锁定部件的实例对象
	UPROPERTY()
	UWarriorWidgetBase* DrawnTargetLockWidget;
	
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
	
};
