// Yu

#include "Items/WarriorProjectileBase.h"
#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGamePlayTags.h"
#include "WarriorDebugHelper.h"
#include "AbilitySystemBlueprintLibrary.h"
// Sets default values
AWarriorProjectileBase::AWarriorProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox=CreateDefaultSubobject<UBoxComponent>(TEXT("ProjectileCollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	
	ProjectileNiagaraComponent=CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComponent"));
	ProjectileNiagaraComponent->SetupAttachment(GetRootComponent());

	ProjectileMovementComp=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	//设置碰撞通道为Pawn、WorldDynamic、WorldStatic可交互，并且发生实际物理碰撞
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);

	
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this,&ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnProjectileBeginOverlap);

	//初始速度
	ProjectileMovementComp->InitialSpeed=700.f;
	ProjectileMovementComp->MaxSpeed=600.f;
	//无重力
	ProjectileMovementComp->ProjectileGravityScale=0.f;
	//控制 Actor 在生成（Spawn）后自动销毁的时间。
	InitialLifeSpan=4.f;
}

// Called when the game starts or when spawned
void AWarriorProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	//只对角色产生效果（即忽视其他）的实现。
	if (ProjectileDamagePolicy == EProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	}
}

void AWarriorProjectileBase::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);
	
	APawn* HitPawn=Cast<APawn>(OtherActor);
	if (!HitPawn || ! UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(),HitPawn))
	{
		//没有击打对象或者不是敌方单位就摧毁子弹
		Destroy();
		return;
	}

	bool bIsValidBlock=false;
	const bool bIsPlayerBlocking= UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitPawn,WarriorGamePlayTags::Player_Status_Blocking);
	if (bIsPlayerBlocking)
	{
		bIsValidBlock=UWarriorFunctionLibrary::IsValidBlock(this,HitPawn);
	}
	
	//GameplayEvent中PayLoad参数
	FGameplayEventData Data;
	Data.Instigator=this;
	Data.Target=HitPawn;
	
	if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitPawn,WarriorGamePlayTags::Player_Event_SuccessfulBlock,Data);
	}
	else
	{
		//Damage
		HandleApplyProjectileDamage(HitPawn,Data);
	}
	//碰撞产生后销毁Projectile
	Destroy();
}

void AWarriorProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedActors.Contains(OtherActor))
	{
		return;
	}

	OverlappedActors.AddUnique(OtherActor);

	if (APawn* HitPawn=Cast<APawn>(OtherActor))
	{
		FGameplayEventData Data;
		Data.Instigator=GetInstigator();
		Data.Target=HitPawn;
		
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(),HitPawn))
		{
			HandleApplyProjectileDamage(HitPawn,Data);
		}
	}
}

void AWarriorProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn,const FGameplayEventData& InPayLoad)
{
	checkf(ProjectileDamageEffectSpecHandle.IsValid(),TEXT("Forgot to assign a valid spec to handle the projectile:%s"),*GetActorNameOrLabel());
	
	const bool bWasApplied=UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor
	(GetInstigator(),InHitPawn,ProjectileDamageEffectSpecHandle);
	
	if (bWasApplied)
	{
		//如果将DamageEffectSpecHandle传入的DealDamageEffect应用，则为InHitPawn发送标签，触发HitReact并传入PayLoad
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor
		(InHitPawn,WarriorGamePlayTags::Shared_Event_HitReact,InPayLoad);
	}
}

