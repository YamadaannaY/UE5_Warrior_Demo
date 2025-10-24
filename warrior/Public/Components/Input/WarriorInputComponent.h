// Yu

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "WarriorInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserObject,typename CallbackFunc>
	void BindNativeInputAction(
		const UDataAsset_InputConfig* InInputConfig,
		const FGameplayTag& InInputTag,
		ETriggerEvent TriggerEvent,
		UserObject* ContextObject,
		CallbackFunc Func);

	//两个CallBackFunc分别在两个BindAction中绑定
	template<class UserObject,typename CallbackFunc>
	void BindAbilityInputAction(
		const UDataAsset_InputConfig* InInputConfig,
		UserObject* ContextObject,
		CallbackFunc InputPressedFunc,
		CallbackFunc InputReleasedFunc);
};

//inline: 允许函数在头文件中被定义，且此头函数被多次包含时不会导致重复定义，而是链接时只保留一次
template <class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig,
	const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	//如果参数为false，则终止并打印信息。
	checkf(InInputConfig,TEXT("Input config data asset is null,can not proceed with binding"));

	//如果在Config结构体中找到对应的IA，BindAction
	if (UInputAction* FoundAction=InInputConfig->FindNativeInputActionByTag(InInputTag))
	{
		BindAction(FoundAction,TriggerEvent,ContextObject,Func);
	}
}


//将IA与CallBackFunc进行绑定，触发时机由ETriggerEvent决定。
template <class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindAbilityInputAction(const UDataAsset_InputConfig* InInputConfig,
	UserObject* ContextObject, CallbackFunc InputPressedFunc, CallbackFunc InputReleasedFunc)
{
	checkf(InInputConfig,TEXT("Input config data asset is null,can not proceed with binding"));
	//遍历存储了GA以及对应Tag的Config结构体数组
	for (const FWarriorInputActionConfig& AbilityInputActionConfig:InInputConfig->AbilityInputActions)
	{
		if (!AbilityInputActionConfig.IsValid()) continue;
		//ETriggerEvent决定了输入动作何时触发回调 Started表明按下按键时触发一次，Completed表明松开时触发一次。
		BindAction(AbilityInputActionConfig.InputAction,ETriggerEvent::Started,ContextObject,InputPressedFunc,
			AbilityInputActionConfig.InputTag);
		BindAction(AbilityInputActionConfig.InputAction,ETriggerEvent::Completed,ContextObject,InputReleasedFunc,
			AbilityInputActionConfig.InputTag);
	}
	
}
