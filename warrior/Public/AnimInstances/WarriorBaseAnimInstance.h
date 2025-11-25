// Yu

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "WarriorBaseAnimInstance.generated.h"
/**
 * 
 */
UCLASS()
class WARRIOR_API UWarriorBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	//在AnimInstance中调用，判断有没有武器Tag来选择执行的Pose
	UFUNCTION(BlueprintPure,meta=(BlueprintThreadSafe))
	bool DoesOwnerHaveTag(FGameplayTag TagToCheck)const ;
	
};
