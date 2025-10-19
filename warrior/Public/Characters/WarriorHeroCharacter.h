// Yu

#pragma once

#include "CoreMinimal.h"
#include "WarriorBaseCharacter.h"
#include "Components/Combat/HeroCombatComponent.h"
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
	virtual void PossessedBy(AController* NewController) override;
	virtual void  BeginPlay() override;
	
	//建立输入映射
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	AWarriorHeroCharacter();

	//~Begin IPawnCombatInterface Interface
	
	//重写接口函数，指向具体类，为接口用
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	/**   End IPawnCombatInterface Interface **/
	
	//~Begin IPawnUIInterface Interface**/

	//重写接口函数，指向具体类，为接口用
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
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

	//SwitchTarget的输入映射
	FVector2D SwitchDirection=FVector2D::ZeroVector;

#pragma endregion

#pragma region Input
	//InputAction、Tag的配置数据资产,为Player独有，因为只有Player具有输入映射
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="CharacterData",meta=(AllowPrivateAccess=true))
	UDataAsset_InputConfig* InputConfigDataAsset;

	//InputTag_Move在ETriggerEvent::Triggered状态下的回调
	void Input_Move(const FInputActionValue &InputActionValue);
	//InputTag_Look在ETriggerEvent::Triggered状态下的回调
	void Input_Look(const FInputActionValue &InputActionValue);
	
	//InputTag_SwitchTarget在ETriggerEvent::Triggered状态下的回调
	void Input_SwitchTargetTriggered(const FInputActionValue &InputActionValue);
	//InputTag_SwitchTarget在ETriggerEvent::Completed状态下的回调
	void Input_SwitchTargetCompleted(const FInputActionValue &InputActionValue);

	//映射输入时调用
	void Input_AbilityInputPressed(FGameplayTag InInputTag);
	//结束映射输入时调用
	void Input_AbilityInputReleased(FGameplayTag InInputTag);
#pragma endregion
public:
	//接口函数重写
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const { return HeroCombatComponent; }
};
