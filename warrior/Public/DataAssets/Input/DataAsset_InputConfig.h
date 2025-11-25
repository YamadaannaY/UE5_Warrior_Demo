// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "DataAsset_InputConfig.generated.h"
/**
 * 
 */
//这个DA将所有IA与Tag绑定，用于接收本地输入激活GA，只有Player需要
UCLASS()
class WARRIOR_API UDataAsset_InputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	//Player默认IMC
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputMappingContext* DefaultMappingContext;

	//IA与Naive_Tag结构体存储数组
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarriorInputActionConfig> NativeInputActions;

	//GA_Tag与IA结构体存储数组
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarriorInputActionConfig> AbilityInputActions;

	//遍历存储了所有IA和对应Tag的NativeInputAction，如果找到输入的Tag，就返回其IA
	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag) const ;
};
