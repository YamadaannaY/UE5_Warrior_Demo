// Yu

#pragma once
//所有tag和InputAction的数据资产。
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "DataAsset_InputConfig.generated.h"
/**
 * 
 */
UCLASS()
class WARRIOR_API UDataAsset_InputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	//Default面板中传入mapping context
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputMappingContext* DefaultMappingContext;

	//Config结构体数组，存储所有本地Tag和InputAction
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarriorInputActionConfig> NativeInputActions;

	//遍历存储了所有InputAction和对应Tag的NativeInputAction，如果找到输入的tag，就返回其InputAction。
	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag) const ;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	//存储所有ability的tag和对应InputAction
	TArray<FWarriorInputActionConfig> AbilityInputActions;
};
