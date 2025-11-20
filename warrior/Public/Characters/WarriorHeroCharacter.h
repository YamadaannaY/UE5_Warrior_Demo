// Yu

#pragma once

#include "CoreMinimal.h"
#include "WarriorBaseCharacter.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "WarriorHeroCharacter.generated.h"

class UAbilityAsync_WaitAttributeChanged;
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
	virtual void  PossessedBy(AController* NewController) override;
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

	UFUNCTION()
	void HandleStaminaChange(FGameplayAttribute Attribute,float NewValue,float OldValue) const ;
	void HandleCurrentHealthChange(FGameplayAttribute Attribute,float NewValue,float OldValue) const ;
	void HandleCurrentRageChange(FGameplayAttribute Attribute,float NewValue,float OldValue) const ;
	
	void InitializeAttributeListener();
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

	UPROPERTY(EditDefaultsOnly,Category="ChangeEffect")
	TSubclassOf<UGameplayEffect> StaminaRegenEffect;

	UPROPERTY(EditDefaultsOnly,Category="ChangeEffect")
	TSubclassOf<UGameplayEffect> CurrentHealthEffect;

	UPROPERTY(EditDefaultsOnly,Category="ChangeEffect")
	TSubclassOf<UGameplayEffect> CurrentRageEffect;

	UPROPERTY(EditDefaultsOnly,Category="ChangeEffect")
	FGameplayTag StaminaRegenEffectTagToRemove;

	UPROPERTY(EditDefaultsOnly,Category="ChangeEffect")
	FGameplayTag CurrentHealthEffectTagToRemove;

	UPROPERTY(EditDefaultsOnly,Category="ChangeEffect")
	FGameplayTag CurrentRageEffectTagToRemove;

	

	int32 AbilityApplyLevel=1;
	
	void HandleStaminaChangeDirect(const FOnAttributeChangeData& Data) const;
	void HandleCurrentHealthChangeDirect(const FOnAttributeChangeData& Data) const;
	void HandleCurrentRageDirect(const FOnAttributeChangeData& Data) const;

#pragma region Input
	//InputAction、Tag的配置数据资产,为Player独有，因为只有Player具有输入映射
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="CharacterData",meta=(AllowPrivateAccess=true))
	UDataAsset_InputConfig* InputConfigDataAsset;

	UPROPERTY()
	FVector2D SwitchDirection=FVector2D::ZeroVector;

	//InputTag_Move在ETriggerEvent::Triggered状态下的回调
	void Input_Move(const FInputActionValue &InputActionValue);
	
	//InputTag_Look在ETriggerEvent::Triggered状态下的回调
	void Input_Look(const FInputActionValue &InputActionValue);
	
	//InputTag_SwitchTarget在ETriggerEvent::Triggered状态下的回调
	void Input_SwitchTargetTriggered(const FInputActionValue &InputActionValue);
	
	//InputTag_SwitchTarget在ETriggerEvent::Completed状态下的回调
	void Input_SwitchTargetCompleted(const FInputActionValue &InputActionValue);

	void Input_PickUp_Stone_Started(const FInputActionValue &InputActionValue);
	
	//映射输入时调用，通过Tag寻找GA，如果找到就激活，或是ToggleAble能力，在状态间进行切换
	void Input_AbilityInputPressed(FGameplayTag InInputTag);
	
	//结束映射输入时调用，只有需要持续输入因此没有手动在GA结尾设置EndAbility的能力需要
	void Input_AbilityInputReleased(FGameplayTag InInputTag);
#pragma endregion

public:
	//接口函数重写
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const { return HeroCombatComponent; }
};
