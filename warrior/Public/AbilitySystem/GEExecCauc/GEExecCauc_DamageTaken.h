// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEExecCauc_DamageTaken.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UGEExecCalc_DamageTaken : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	//绑定GE:SharedDealDamage
	UGEExecCalc_DamageTaken();
	
	//当某个GameplayEffect配置了此GEE并且被应用到目标，会自动调用此函数。
	virtual void  Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput)const override;
};
