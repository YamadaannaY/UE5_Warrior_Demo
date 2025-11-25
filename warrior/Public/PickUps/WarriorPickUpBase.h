// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorPickUpBase.generated.h"

class USphereComponent;

UCLASS()
class WARRIOR_API AWarriorPickUpBase : public AActor
{
	GENERATED_BODY()
	
public:
	
	AWarriorPickUpBase();
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Pick Up Interaction")
	USphereComponent* PickUpCollisionSphere;
	
protected:
	//当拾取物触发Overlap时的回调
	UFUNCTION()
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

};
