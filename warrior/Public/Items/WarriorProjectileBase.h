// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "WarriorProjectileBase.generated.h"

struct FGameplayEventData;
class UBoxComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;

UENUM(BlueprintType)
enum class EProjectileDamagePolicy : uint8
{
	OnHit,
	OnBeginOverlap
};

UCLASS()
class WARRIOR_API AWarriorProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWarriorProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//碰撞体积
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UBoxComponent* ProjectileCollisionBox;

	//飞行物粒子特效（飞行物实体就是Box，所以粒子外观就是子弹）
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UNiagaraComponent* ProjectileNiagaraComponent;

	//UE内置飞行物运动组件，可设置飞行物各运动参数
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UProjectileMovementComponent* ProjectileMovementComp;

	//默认造成伤害的逻辑是碰撞产生
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	EProjectileDamagePolicy ProjectileDamagePolicy=EProjectileDamagePolicy::OnHit;

	//通过GE实现火球伤害
	UPROPERTY(BlueprintReadOnly,Category="Projectile",meta=(ExposeOnSpawn="true"))
	FGameplayEffectSpecHandle ProjectileDamageEffectSpecHandle;
	


	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

	UFUNCTION()
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent,meta=(Displayname="On Spawn Projectile FX"))
	void BP_OnSpawnProjectileHitFX(const FVector& HitLocation);

private:
	void HandleApplyProjectileDamage(APawn* InHitPawn,const FGameplayEventData& InPayLoad);
};
