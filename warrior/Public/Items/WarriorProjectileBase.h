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
	AWarriorProjectileBase();

protected:
	virtual void BeginPlay() override;

	//碰撞体积
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UBoxComponent* ProjectileCollisionBox;

	//飞行物粒子特效（飞行物实体是Box，所以粒子特效就是子弹外观）
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UNiagaraComponent* ProjectileNiagaraComponent;

	//UE内置运动组件，可设置Projectile各运动参数
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	UProjectileMovementComponent* ProjectileMovementComp;

	//默认造成伤害的逻辑是碰撞
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Projectile")
	EProjectileDamagePolicy ProjectileDamagePolicy=EProjectileDamagePolicy::OnHit;

	//在Spawn时将其暴露，传入GE:SharedDamage的SpecHandle
	UPROPERTY(BlueprintReadOnly,Category="Projectile",meta=(ExposeOnSpawn="true"))
	FGameplayEffectSpecHandle ProjectileDamageEffectSpecHandle;
	
	//伤害造成策略一绑定的逻辑函数
	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

	//伤害造成策略二绑定的逻辑函数
	UFUNCTION()
	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//逻辑由蓝图实现，传入一个碰撞位置作为参数，在位置生成音效和粒子特效
	UFUNCTION(BlueprintImplementableEvent,meta=(Displayname="On Spawn Projectile FX"))
	void BP_OnSpawnProjectileHitFX(const FVector& HitLocation);

private:
	//ApplyGE 到HitPawn
	void HandleApplyProjectileDamage(APawn* InHitPawn,const FGameplayEventData& InPayLoad);
};
