// Yu

#pragma once

#include "CoreMinimal.h"
#include "WarriorBaseCharacter.h"
#include "WarriorEnemyCharacter.generated.h"

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

	//重写接口函数，用于外部调用接口时使用
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	
	//~End IPawnCombatInterface Interface

	//~Begin IPawnUIInterface Interface**/

	//重写接口的纯虚函数，用于外部调用接口时使用
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	//这个函数给自己内部使用
	virtual UEnemyUIComponent* GetEnemyUIComponent() const override;

	//~End IPawnUIInterface Interface**/

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat");
	UEnemyCombatComponent* EnemyCombatComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UEnemyUIComponent* EnemyUIComponent;

	//这个组件不像Player的UI，它绑定在Enemy->GetMesh()上，是一个具体可见的生命值Widget组件。
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UWidgetComponent* EnemyHealthWidgetComponent;

	//~Begin APawn Interface
	
	virtual void PossessedBy(AController* NewController) override;

	//~End APwn Interface

private:

	//初始化EnemyStartUpData
	void InitEnemyStartUpData() const;

public:

	//获取自身EnemyCombatComponent()
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const {return EnemyCombatComponent;}
};
