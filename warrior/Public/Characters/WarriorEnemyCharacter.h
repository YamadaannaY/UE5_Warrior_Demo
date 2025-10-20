// Yu

#pragma once

#include "CoreMinimal.h"
#include "WarriorBaseCharacter.h"
#include "WarriorEnemyCharacter.generated.h"

class UBoxComponent;
class UWidgetComponent;
class UEnemyCombatComponent;
/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorEnemyCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()
public:
	AWarriorEnemyCharacter();

	//~Begin IPawnCombatInterface Interface 

	//重写接口函数，指向具体类
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	
	//~End IPawnCombatInterface Interface

	//~Begin IPawnUIInterface Interface**/

	//重写接口函数，指向具体类，为接口用
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual UEnemyUIComponent* GetEnemyUIComponent() const override;

	//~End IPawnUIInterface Interface**/

	//~Begin UObject Interface**/

#if WITH_EDITOR
	// 在编辑器中属性发生变化时自动调用
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~End  UObject Interface**/

protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat");
	UEnemyCombatComponent* EnemyCombatComponent;

	//左手的CollisionBoxBox
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat");
	UBoxComponent* LeftHandCollisionBox;

	//左手CollisionBoxBox所Attach的骨骼的名字
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat");
	FName LeftHandCollisionBoxAttachBoneName;

	//右手的CollisionBoxBox
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat");
	UBoxComponent* RightHandCollisionBox;

	//右手CollisionBoxBox所Attach的骨骼的名字
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat");
	FName RightHandCollisionBoxAttachBoneName;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UEnemyUIComponent* EnemyUIComponent;

	//这个组件绑定在Enemy->GetMesh()上，是一个具体可见的Widget组件。
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UWidgetComponent* EnemyHealthWidgetComponent;

	//处理碰撞的委托
	UFUNCTION()
	virtual void OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//~Begin APawn Interface
	
	virtual void PossessedBy(AController* NewController) override;

	//~End APwn Interface

private:

	//初始化EnemyStartUpData
	void InitEnemyStartUpData() const;

public:

	//内联小接口函数
	
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const {return EnemyCombatComponent;}
	FORCEINLINE UBoxComponent* GetLeftHandCollisionBox() const {return LeftHandCollisionBox;}
	FORCEINLINE UBoxComponent* GetRightHandCollisionBox() const {return RightHandCollisionBox;}
};
