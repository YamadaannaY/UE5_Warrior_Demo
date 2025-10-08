// Yu

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/PawnUIInterface.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/PawnCombatInterface.h"
#include "WarriorBaseCharacter.generated.h"
class UMotionWarpingComponent;
class  UDataAsset_StartUpDataBase;

UCLASS()
//IAbilitySystemInterface:提供一个接口，让外部获得角色的ability system
class WARRIOR_API AWarriorBaseCharacter : public ACharacter,public IAbilitySystemInterface,public IPawnCombatInterface,public IPawnUIInterface
{
	GENERATED_BODY()
public:
	AWarriorBaseCharacter();
	
	// Begin IAbilitySystemInterface Interface
	
	//Character下获得ASC
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override ;
	//Begin IAbilitySystemInterface Interface
	
	//Begin IPawnCombatInterface Interface**/
	
	//Character下获得CombatComponent
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	
	//Begin IPawnCombatInterface Interface**/

	//Begin IPawnUIInterface Interface**/
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	//Begin IPawnUIInterface Interface**/

protected:
	//~ begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ end APawn Interface.

	//ASC组件
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AbilitySystem")
	UWarriorAbilitySystemComponent* WarriorAbilitySystemComponent;

	//属性集
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AbilitySystem")
	UWarriorAttributeSet* WarriorAttributeSet;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="MotionWarping")
	UMotionWarpingComponent* MotionWarpingComponent;

	//软引用的一个数据资产，存储角色初始数据（如不需要触发就拥有的OnGiven能力），
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="CharacterData",meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;

public:
	//强制内联，减少调用开销，用于高频使用的小函数，相比inline更加强制，且跨平台统一
	//为接口准备的函数，通过接口调用函数获得GAS系统数据
	
	//Character下获得WarriorAbilitySystemComponent()
	FORCEINLINE UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const { return WarriorAbilitySystemComponent; }
	//Character下获得AttributeSet
	FORCEINLINE UWarriorAttributeSet* GetWarriorAttributeSet() const { return WarriorAttributeSet; }
	
	
};
