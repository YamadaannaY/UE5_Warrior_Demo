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

	// Begin IPawnCombatInterface Interface 

	//Enemy下获得PawnCombatComponent
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	
	//End IPawnCombatInterface Interface

	//Begin IPawnUIInterface Interface**/

	//重写接口的纯虚函数，用于Character自身获得UIComponent
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	//这个函数作为给外部的接口，当外部调用EnmeyCharacter需要UI组件时使用
	virtual UEnemyUIComponent* GetEnemyUIComponent() const override;

	//Begin IPawnUIInterface Interface**/

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat");
	UEnemyCombatComponent* EnemyCombatComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UEnemyUIComponent* EnemyUIComponent;

	//这个组件不像Player的Widget，它绑定在Enemy->GetMesh()上，是一个具体可见的生命值组件。
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI")
	UWidgetComponent* EnemyHealthWidgetComponent;

	//~Begin APawn Interface
	
	virtual void PossessedBy(AController* NewController) override;

	//~End APwn Interface

private:

	//初始化EnemyStartUpData
	void InitEnemyStartUpData() const;

public:

	//外部接口下获得获取EnemyCombatComponent()
	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const {return EnemyCombatComponent;}
};
