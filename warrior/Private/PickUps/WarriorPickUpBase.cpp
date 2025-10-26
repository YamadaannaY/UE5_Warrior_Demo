// Yu


#include "PickUps/WarriorPickUpBase.h"

#include "Components/SphereComponent.h"

// Sets default values
AWarriorPickUpBase::AWarriorPickUpBase()
{

	PrimaryActorTick.bCanEverTick = false;
	
	PickUpCollisionSphere=CreateDefaultSubobject<USphereComponent>("PickUpCollisionSphere");
	SetRootComponent(PickUpCollisionSphere);
	PickUpCollisionSphere->InitSphereRadius(50.f);
	PickUpCollisionSphere->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnPickUpCollisionSphereBeginOverlap);

}

void AWarriorPickUpBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
}
