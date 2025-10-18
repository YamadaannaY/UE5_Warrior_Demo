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
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
	/**两种不同策略下的委托回调 **/
	//对所有物体都是碰撞下产生逻辑
	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this,&ThisClass::OnProjectileHit);
	//如果设置了EProjectileDamagePolicy::OnBeginOverlap，则对角色的策略改变为重叠交互，也就是穿透其他环境（因为其他的是Block），但是对于角色有作用
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnProjectileBeginOverlap);

	//初始速度
	ProjectileMovementComp->InitialSpeed=700.f;
	ProjectileMovementComp->MaxSpeed=600.f;
	//暂时无重力
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
	//PayLoad
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
	Destroy();
}

void AWarriorProjectileBase::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AWarriorProjectileBase::HandleApplyProjectileDamage(APawn* InHitPawn,const FGameplayEventData& InPayLoad)
{
	checkf(ProjectileDamageEffectSpecHandle.IsValid(),TEXT("Forgot to assign a valid spec to handle the projectile:%s"),*GetActorNameOrLabel());
	const bool bWasApplied=UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor
	(GetInstigator(),InHitPawn,ProjectileDamageEffectSpecHandle);
	
	if (bWasApplied)
	{
		//如果将DamageEffectSpecHandle传入的DealDamageEffect应用，则为InHitPawn发送标签，触发GA：HitReact
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor
		(InHitPawn,WarriorGamePlayTags::Shared_Event_HitReact,InPayLoad);
	}
}

