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

	//Config结构体数组，存储依靠纯逻辑实现的本地IA与对应Tag（eg：Look、Move都是依靠向量计算完成操作）
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarriorInputActionConfig> NativeInputActions;

	//遍历存储了所有IA和对应Tag的NativeInputAction，如果找到输入的Tag，就返回其IA
	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag) const ;
	
	//存储所有GATag和对应IA
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarriorInputActionConfig> AbilityInputActions;
};
