// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WarriorTypes/WarriorEnumType.h"
#include "WarriorFunctionLibrary.generated.h"

class UPawnCombatComponent;
struct FGameplayTag;
class UWarriorAbilitySystemComponent;

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

	//如果Actor没有Matching Tag ,则添加TagToAdd
	UFUNCTION(BlueprintCallable,Category="Warrior|Function Library")
	static void AddGameplayTagToActorIfNone(AActor* InActor,FGameplayTag TagToAdd);

	//如果Actor有Matching Tag ,则删除GA
	UFUNCTION(BlueprintCallable,Category="Warrior|Function Library")
	static void RemoveGameplayFromActorIfFound(AActor* InActor,FGameplayTag TagToRemove);

	//工具函数,检查是否Actor有TagToCheck，用于BP_DoesActorHaveTag
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

	
};
