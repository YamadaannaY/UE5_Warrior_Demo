#pragma once
//纯c++ 枚举变量

UENUM()
enum class EWarriorConfirmType :uint8
{ Yes,No};

UENUM()
enum class EWarriorValidType :uint8
{
	Valid,
	Invalid
};

//枚举，两个变量，需要输入触发与不需要
UENUM(BlueprintType)
enum class EWarriorAbilityActivationPolicy : uint8
{
	OnTriggered,
	OnGiven
};

UENUM()
enum class EWarriorSuccessType: uint8
{
	Successful,
	Failed
};

UENUM(BlueprintType)
enum class EToggleDamageType :uint8
{
	CurrentEquippedWeapon,
	LeftHand,
	RightHand
};