// Yu

#pragma once

#include "CoreMinimal.h"
#include "WarriorBaseCharacter.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Components/UI/HeroUIComponent.h"
#include "WarriorHeroCharacter.generated.h"
struct FInputActionValue;
class UDataAsset_InputConfig;
class InputActionValue;
class UHeroCombatComponent;
class UHeroUIComponent;
 /*
 */
UCLASS()
class WARRIOR_API AWarriorHeroCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()
protected:
	//判断：如果有StratUpData且同步加载成功(StartUpData软引用)，则生成所有能力。
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void  BeginPlay() override;
	
	//建立输入映射
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	AWarriorHeroCharacter();

	//~Begin IPawnCombatInterface Interface
	
	//获得CombatComponent
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	/**   End IPawnCombatInterface Interface **/
	
	//~Begin IPawnUIInterface Interface**/

	//外部接口用
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	//本地用
	virtual UHeroUIComponent* GetHeroUIComponent() const override;

	//~End IPawnUIInterface Interface**/
private:
#pragma region Components
	//相机臂
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Camera",meta=(AllowPrivateAccess=true))
	class USpringArmComponent* CameraBoom;

	//相机
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Camera",meta=(AllowPrivateAccess=true))
	class UCameraComponent* FollowCamera;

	//玩家战斗组件
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat",meta=(AllowPrivateAccess=true))
	UHeroCombatComponent* HeroCombatComponent;

	//玩家UI组件
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="UI",meta=(AllowPrivateAccess=true))
	UHeroUIComponent* HeroUIComponent;

#pragma endregion

#pragma region Input
	//InputAction、tag的配置数据资产
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="CharacterData",meta=(AllowPrivateAccess=true))
	UDataAsset_InputConfig* InputConfigDataAsset;

	//InputTag_Move在ETriggerEvent::Triggered状态下的回调函数
	void Input_Move(const FInputActionValue &InputActionValue);
	//InputTag_Look在ETriggerEvent::Triggered状态下的回调函数
	void Input_Look(const FInputActionValue &InputActionValue);
	
	//InputTag_SwitchTarget在ETriggerEvent::Triggered状态下的回调函数
	void Input_SwitchTargetTriggered(const FInputActionValue &InputActionValue);
	//InputTag_SwitchTarget在ETriggerEvent::Completed状态下的回调函数
	void Input_SwitchTargetCompleted(const FInputActionValue &InputActionValue);

	FVector2D SwitchDirection=FVector2D::ZeroVector;

	//输入时调用逻辑
	void Input_AbilityInputPressed(FGameplayTag InInputTag);
	//结束输入时调用逻辑
	void Input_AbilityInputReleased(FGameplayTag InInputTag);
#pragma endregion
public:
	//接口函数，获得HeroCombatComponent
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const { return HeroCombatComponent; }
};
