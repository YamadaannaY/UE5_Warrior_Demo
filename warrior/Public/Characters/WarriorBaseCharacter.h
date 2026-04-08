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
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override ;
	
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	
protected:
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AbilitySystem")
	EGameplayEffectReplicationMode AscReplicationMode=EGameplayEffectReplicationMode::Mixed;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AbilitySystem")
	UWarriorAbilitySystemComponent* WarriorAbilitySystemComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AbilitySystem")
	UWarriorAttributeSet* WarriorAttributeSet;

	//运动扭曲组件，用于改正角色朝向
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="MotionWarping")
	UMotionWarpingComponent* MotionWarpingComponent;

	//软引用的一个数据资产，存储角色的基本ASC数据（GA,GE）
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="CharacterData",meta=(AllowPrivateAccess=true))
	TSoftObjectPtr<UDataAsset_StartUpDataBase> CharacterStartUpData;

public:
	FORCEINLINE UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const { return WarriorAbilitySystemComponent; }
	
	FORCEINLINE UWarriorAttributeSet* GetWarriorAttributeSet() const { return WarriorAttributeSet; }

	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
};
