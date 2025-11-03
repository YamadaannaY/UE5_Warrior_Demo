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

//GA激活策略  
UENUM(BlueprintType)
enum class EWarriorAbilityActivationPolicy : uint8
{	//1.需要输入触发
	OnTriggered,
	//2.ASC.GiveAbility(Spec.Handle)后直接激活能力
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
//CoolDown的输入引脚
UENUM()
enum class EWarriorCountDownActionInput: uint8
{
	Start,
	Cancel
};

//CoolDown的输出引脚
UENUM()
enum class EWarriorCountDownActionOutput: uint8
{
	Updated,
	Completed,
	Cancelled
};

UENUM(BlueprintType)
enum class EWarriorGameDifficulty : uint8
{
	Easy,
	Normal,
	Hard,
	VeryHard
};

UENUM()
enum class EWarriorInputMode :uint8
{
	GameOnly,
	UIOnly
};