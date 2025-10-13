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
	//将具体的Func实现与config中的InputAction一一关联，桥梁是tag。
	//tag的意义：用标签代替复杂的状态机，根据标签实现各种条件，而且方便拓展，加新tag而不是修改原先代码实现新功能
	template<class UserObject,typename CallbackFunc>
	void BindNativeInputAction(
		const UDataAsset_InputConfig* InInputConfig,
		const FGameplayTag& InInputTag,
		ETriggerEvent TriggerEvent,
		UserObject* ContextObject,
		CallbackFunc Func);

	//实现InputAction绑定：遍历InInputConfig : AbilityInputActions 结构体，这个数组存储了所有ability的tag和对应InputAction，
	//将AbilityInputAction 与InputPressedFunc InputReleasedFunc 相关联
	template<class UserObject,typename CallbackFunc>
	void BindAbilityInputAction(
		const UDataAsset_InputConfig* InInputConfig,
		UserObject* ContextObject,
		CallbackFunc InputPressedFunc,
		CallbackFunc InputReleasedFunc);
};

//inline: 允许函数在头文件中被定义，且此头函数被多次包含时不会导致重复定义，而是链接时只保留一次

//寻找InputTag对应的InputAction，找到则与CallBackFunc进行BindAction
template <class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig,
	const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	//check(InInputConfig);
	//如果参数为false，则终止并打印信息。
	checkf(InInputConfig,TEXT("Input config data asset is null,can not proceed with binding"));

	//如果在Config中找到对应的InputAction，则将BindAction
	if (UInputAction* FoundAction=InInputConfig->FindNativeInputActionByTag(InInputTag))
	{
		BindAction(FoundAction,TriggerEvent,ContextObject,Func);
	}
}


//将AbilityInputAction与CallbackFunc进行BindAction
template <class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindAbilityInputAction(const UDataAsset_InputConfig* InInputConfig,
	UserObject* ContextObject, CallbackFunc InputPressedFunc, CallbackFunc InputReleasedFunc)
{
	checkf(InInputConfig,TEXT("Input config data asset is null,can not proceed with binding"));
	//遍历存储了能力以及对应Tag的数组
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
