// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarriorTypes/WarriorEnumType.h"
#include "WarriorFunctionLibrary.generated.h"

class UWarriorGameInstance;
class UPawnCombatComponent;
struct FGameplayTag;
class UWarriorAbilitySystemComponent;
struct FGameplayEffectSpecHandle;

/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//工具函数，获得Actor对应的ASC
	static UWarriorAbilitySystemComponent* NativeGetWarriorASCFromActor(AActor* InActor);

	//如果Actor没有TagToAdd,则添加
	UFUNCTION(BlueprintCallable,Category="Warrior|Function Library")
	static void AddGameplayTagToActorIfNone(AActor* InActor,FGameplayTag TagToAdd);

	//如果Actor有TagToRemove,则删除
	UFUNCTION(BlueprintCallable,Category="Warrior|Function Library")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor,FGameplayTag TagToRemove);

	//工具函数,检查是否Actor有TagToCheck
	static bool NativeDoesActorHaveTag(AActor* InActor,FGameplayTag TagToCheck);

	//蓝图函数，判断Tag是否被包含，参数为actor和Tag，返回Yes /No
	UFUNCTION(BlueprintCallable,Category="Warrior|Function Library",meta=(DisplayName="Does Actor Has Tag",ExpandEnumAsExecs="OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor,FGameplayTag TagToCheck,EWarriorConfirmType& OutConfirmType);

	//工具函数，获取PawnCombatComponent，用于 BP_GetPawnCombatComponentFromActor
	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);

	//蓝图函数，获取Actor的PawnCombatComponent，且具有两个输出引脚（Valid/Invalid）
	UFUNCTION(BlueprintCallable,Category="Warrior|Function Library",meta=(DisplayName="GetPawnCombatComponentFromActor",ExpandEnumAsExecs="OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, EWarriorValidType& OutValidType );

	//判断攻击者与被攻击者是不是同一个ID
	UFUNCTION(BlueprintPure,Category="Warrior|Function Library")
	static bool IsTargetPawnHostile(APawn* QueryPawn,APawn* TargetPawn);

	//计算二者角度差值，根据值判断DirectionTag以触发对应的Montage
	UFUNCTION(BlueprintPure,Category="Warrior|Function Library")
	static FGameplayTag ComputeHitReactDirectionTag(AActor* InAttacker,AActor* InVictim,float& OutAngleDifference);
	
	//根据角度判断格挡是否生效
	UFUNCTION(BlueprintPure,Category="Warrior|Function Library")
	static bool IsValidBlock(AActor* InAttacker,AActor* InDefender);

	UFUNCTION(BlueprintCallable,Category="Warrior|Function Library")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator,AActor* InTargetActor,const FGameplayEffectSpecHandle& InSpecHandle);

	//静态函数，找到倒计时逻辑函数（如果没有则创建一个）并进行包装
	//property说明：蓝图可调用，声明是一个延时函数，指定在哪个世界上下文执行操作，告诉引擎LatenInfo参数是用于内部管理延迟状态的特殊参数，将两个枚举类型展开为执行引脚，没有引用的为输入引脚，有引用的为输出（注：常量引用也是输入引脚）
	UFUNCTION(BlueprintCallable,Category="Warrior|Function Library",meta=(Latent,WorldContext="WorldContextObject",LatentInfo="LatenInfo",ExpandEnumAsExecs="CountDownInput|CountDownOutput",TotalTime="1.0",UpdateInterval="0.1"))
	static void CountDown(const UObject* WorldContextObject,float TotalTime,float UpdateInterval,float& OutRemainingTime,EWarriorCountDownActionInput CountDownInput,
		UPARAM(DisplayName="Output") EWarriorCountDownActionOutput& CountDownOutput,FLatentActionInfo LatenInfo);

	//获取GameInstance
	UFUNCTION(BlueprintPure,Category="Warrior|Function Library",meta=(WorldContext="WorldContextObject"))
	static UWarriorGameInstance* GetWarriorGameInstance(const UObject* WorldContextObject);

	//切换Input模式：UI和Game
	UFUNCTION(BlueprintCallable,Category="Warrior|Function Library",meta=(WorldContext="WorldContextObject"))
	static void ToggleInputMode(const UObject* WorldContextObject,EWarriorInputMode InInputMode);

	//保存GameDifficulty到SavedCurrentDifficulty
	UFUNCTION(BlueprintCallable,Category="Warrior|Function Library")
	static void SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficultyToSave);
	
	//将SavedCurrentDifficulty加载为GameDifficulty
	UFUNCTION(BlueprintCallable,Category="Warrior|Function Library")
	static bool TryLoadSavedGameDifficulty(EWarriorGameDifficulty& OutSaveDifficulty);
};

