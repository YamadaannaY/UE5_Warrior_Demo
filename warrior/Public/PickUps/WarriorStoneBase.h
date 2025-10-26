// Yu

#pragma once

#include "CoreMinimal.h"
#include "PickUps/WarriorPickUpBase.h"
#include "WarriorStoneBase.generated.h"

class UGameplayEffect;
class UWarriorAbilitySystemComponent;
/**
 * 
 */

UCLASS()
class WARRIOR_API AWarriorStoneBase : public AWarriorPickUpBase
{
	GENERATED_BODY()
public:
	void Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent,int32 ApplyLevel);

protected:
	//BeginOverlap事件的回调，意味着可以产生交互，触发交互GA收集Stone
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	//在角色位置生成音效特效
	UFUNCTION(BlueprintImplementableEvent,meta=(DisplayName="On Stones Consumed"))
	void BP_OnStoneConsumed();

	//传入的GE，用于实现特定物品的属性效果
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StoneGameplayEffectClass;
};
