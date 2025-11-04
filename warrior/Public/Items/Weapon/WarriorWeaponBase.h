// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorWeaponBase.generated.h"

class UBoxComponent;

//自定义一个带AActor*参数，委托名为FOnTargetInteractedDelegate的Delegate
DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate,AActor*);

UCLASS()
class WARRIOR_API AWarriorWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:
	AWarriorWeaponBase();

	//BeginOverlap的回调函数中调用此委托，以调用OnWeaponHitTargetActor
	FOnTargetInteractedDelegate OnWeaponHitTarget;
	//EndOverlap的回调函数中调用此委托，以调用 OnWeaponPulledFromTargetActor
	FOnTargetInteractedDelegate OnWeaponPulledFromTarget;
	
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Weapons")
	//武器Mesh
	UStaticMeshComponent* WeaponMesh;
	//碰撞box
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Weapons")
	UBoxComponent* WeaponCollisionBox;

	//监听OnComponentBeginOverlap进行回调的函数，此函数包含所有需要在碰撞开始时发生的事件
	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//监听OnComponentEndOverlap进行回调的函数，此函数包含所有需要在碰撞结束时发生的事件
	UFUNCTION()
	virtual void OnCollisionBoxEndOverlap(UPrimitiveComponent*OverlappedComponent, AActor*OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:
	//获得WeaponCollisionBox
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const {return WeaponCollisionBox;}


};
