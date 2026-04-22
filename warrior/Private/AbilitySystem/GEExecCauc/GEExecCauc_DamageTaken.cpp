// Yu

#include "AbilitySystem/GEExecCauc/GEExecCauc_DamageTaken.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "WarriorGamePlayTags.h"

//封装Capture
struct FWarriorDamageCapture
{	
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)
	
	FWarriorDamageCapture()
	{

		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet,AttackPower,Source,false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet,DefensePower,Target,false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet,DamageTaken,Target,false)
	}
};

//静态工具函数，用于获取FWarriorDamageCapture对象;
static const FWarriorDamageCapture& GetWarriorDamageCapture()
{
	static FWarriorDamageCapture WarriorDamageCapture;
	return WarriorDamageCapture;
}


UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	//将属性包括其定义规则组成的Def加入Capture的数组中，这些属性被用于计算逻辑
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DamageTakenDef);
}

void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//获得当前正在执行的EffectSpec
	const FGameplayEffectSpec& EffectSpec=ExecutionParams.GetOwningSpec();

	//FAggregatorEvaluateParameters主要作用在于判断是否有BuffTag，作为计算值参与计算，暂时还没有Buff，当一个参数即可
	//SourceTags → 施法者ASC的标签集合
	//TargetTags → 目标ASC的标签集合
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags=EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags=EffectSpec.CapturedTargetTags.GetAggregatedTags();
	
	//自定义存储值
	float SourceAttackPower=0.f;
	float TargetDefensePower=0.f;
	
	//尝试取出属性值并赋给存储值
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().AttackPowerDef, EvaluateParameters,SourceAttackPower);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().DefensePowerDef, EvaluateParameters,TargetDefensePower);
	
	float BaseDamage=0.f;
	int32 UsedLightAttackComboCount=0;
	int32 UsedHeavyAttackComboCount=0;
	
	//SetSetByCallerMagnitude→是一种动态参数传递方式，调用时由 Ability/代码来设置SetByCallerMagnitude
	//动态变化的值用此方法存储
	for (const TPair<FGameplayTag,float>& TagMagnitude:EffectSpec.SetByCallerTagMagnitudes)
	{
		//在GESpecHandle.Data中SetByCallerTagMagnitudes绑定Tag与对应值，将返回的值获取伤害
		if (TagMagnitude.Key.MatchesTagExact(WarriorGamePlayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage=TagMagnitude.Value;
		}
		
		//固定的乘积系数，根据轻重击获取不同参数
		if (TagMagnitude.Key.MatchesTagExact(WarriorGamePlayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedLightAttackComboCount=TagMagnitude.Value;
		}
		if (TagMagnitude.Key.MatchesTagExact(WarriorGamePlayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedHeavyAttackComboCount=TagMagnitude.Value;
		}
	}
	
	//轻击段数基础上的伤害系数增比
	if (UsedLightAttackComboCount != 0)
	{
		const float DamageIncreasePercentLight=UsedLightAttackComboCount*0.08+1.f;
		BaseDamage*=DamageIncreasePercentLight;
	}
	//重击段数基础上的伤害系数增比
	if (UsedHeavyAttackComboCount!=0)
	{
		const float DamageIncreasePercentHeavy=UsedHeavyAttackComboCount*0.25f+1.f;
		BaseDamage*=DamageIncreasePercentHeavy;
	}
	const float MultAttack=FMath::RandRange(0.8,1.2);
	
	//最终伤害判定公式
	const float FinalDamageDone=(BaseDamage+MultAttack*SourceAttackPower)*(100/(100+TargetDefensePower));

	//将这个值赋予属性集
	if (FinalDamageDone >0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetWarriorDamageCapture().DamageTakenProperty,EGameplayModOp::Override,FinalDamageDone));
	}
}
