// Yu


#include "Items/Weapon/WarriorWeaponBase.h"
#include "Components/BoxComponent.h"

AWarriorWeaponBase::AWarriorWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponCollisionBox=CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(WeaponMesh);
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	//默认下无碰撞。
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//当物体开始/结束重叠时，执行回调函数。
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnCollisionBoxBeginOverlap);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this,&ThisClass::OnCollisionBoxEndOverlap);
}

void AWarriorWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//获得发起者，这里是角色
	const APawn* WeaponOwningPawn=GetInstigator<APawn>();
	checkf(WeaponOwningPawn,TEXT("Forgot to assign an Instigator as te owning pawn for the weapon:%s "),*GetName());

	if (const APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		//不想让Enemy能够伤害彼此，所以需要判断pawn类不是同一个。
		if (WeaponOwningPawn!=HitPawn)
		{
			//执行此委托绑定的函数并且传入参数OtherActor，即碰撞对象。
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
		}
	}
}

void AWarriorWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const APawn* WeaponOwningPawn=GetInstigator<APawn>();
	checkf(WeaponOwningPawn,TEXT("Forgot to assign an Instigator as te owning pawn for the weapon:%s "),*GetName());
	if (const APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (WeaponOwningPawn!=HitPawn)
		{
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
		}
	}
}
