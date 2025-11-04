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
//IAbilitySystemInterface:提供一个接口，让外部获得角色的AbilitySystem
class WARRIOR_API AWarriorBaseCharacter : public ACharacter,public IAbilitySystemInterface,public IPawnCombatInterface,public IPawnUIInterface
{
	GENERATED_BODY()
public:
	AWarriorBaseCharacter();
	
	//~ Begin IAbilitySystemInterface Interface
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override ;
	
	//~End IAbilitySystemInterface Interface
	
	//~Begin IPawnCombatInterface Interface**/
	
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	
	//~End IPawnCombatInterface Interface**/
	
	//~Begin IPawnUIInterface Interface**/
	
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	
	//~End IPawnCombatInterface Interface**/
protected:
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface.

	//ASC组件
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AbilitySystem")
	UWarriorAbilitySystemComponent* WarriorAbilitySystemComponent;

	//属性集
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AbilitySystem")
	UWarriorAttributeSet* WarriorAttributeSet;

	//运动扭曲组件，用于改正角色朝向
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="MotionWarping")
	UMotionWarpingComponent* MotionWarpingComponent;

	//软引用的一个数据资产，存储角色初始数据（如不需要触发就拥有的OnGiven能力），
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="CharacterData",meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;

public:
	//Character下获得WarriorAbilitySystemComponent()
	FORCEINLINE UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const { return WarriorAbilitySystemComponent; }
	//Character下获得AttributeSet
	FORCEINLINE UWarriorAttributeSet* GetWarriorAttributeSet() const { return WarriorAttributeSet; }
};
