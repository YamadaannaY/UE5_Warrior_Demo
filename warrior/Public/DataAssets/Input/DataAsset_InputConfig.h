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
	//DA_Default面板中设置IMC
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputMappingContext* DefaultMappingContext;

	//存储所有基础的IA，即不会在IMC中被替换的能力
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarriorInputActionConfig> NativeInputActions;

	//存储所有GATag和对应IA进行绑定
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarriorInputActionConfig> AbilityInputActions;

	//遍历存储了所有IA和对应Tag的NativeInputAction，如果找到输入的Tag，就返回其IA
	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag) const ;
};
