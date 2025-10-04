// Yu

#include "AbilitySystem/GEExecCauc/GEExecCauc_DamageTaken.h"
#include "AbilitySystem/WarriorAttributeSet.h"
#include "WarriorDebugHelper.h"
#include "WarriorGamePlayTags.h"
/** 绑定了GE，从**/

struct FWarriorDamageCapture
{	//等效于slow way中的捕获操作，声明要在属性集中寻找的的属性
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)
	FWarriorDamageCapture()
	{
		//等效于slow way 中为捕获属性进行规则定义的操作。
		//从UWarriorAttributeSet中寻找特定属性，定义属于Source（player）还是Target（enemy），是否每次都计算新的值，false则是，true则只看第一次输入值
		//定义后生成Def，表明定义。
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet,AttackPower,Source,false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet,DefensePower,Target,false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet,DamageTaken,Target,false)
	}
};

//静态工具函数，用于获取FWarriorDamageCapture对象,调用Def;
static const FWarriorDamageCapture& GetWarriorDamageCapture()
{
	static FWarriorDamageCapture WarriorDamageCapture;;
	return WarriorDamageCapture;
}


UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	/*Slow way to doing capture
	//指向UWarriorAttributeSet类中AttackPower属性的一个反射指针,即捕获此属性
	FProperty* AttackPowerProperty=FindFieldChecked<FProperty>(UWarriorAttributeSet::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UWarriorAttributeSet,AttackPower));
		
	//它定义了一个 属性捕获规则，用于在 GameplayEffect计算公式里读取某个属性。
	//EGameplayEffectAttributeCaptureSource定义从Source还是Target中获取值
	//true：在应用时刻固定值，然后不再更新 false：在每一次进行计算时动态读取当前值（有buff改变数值时）
	
	FGameplayEffectAttributeCaptureDefinition AttackPowerCaptureDefinition(AttackPowerProperty,EGameplayEffectAttributeCaptureSource::Source,false);
	
	RelevantAttributesToCapture.Add(AttackPowerCaptureDefinition); */
	
	//将属性包括其定义规则组成的Def加入Capture的数组中，这些属性被用于计算逻辑
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DamageTakenDef);

}

//FGameplayEffectCustomExecutionParameters包含了正在执行的GA的上下文，比如施法者、目标、捕获的属性等。
//OutExecutionOutput → 输出参数，用来写入最终的计算结果（比如最终伤害、添加的修饰符等）。
void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//获得当前正在执行的GA的Spec(eg:轻击GA，重击GA)
	const FGameplayEffectSpec& EffectSpec=ExecutionParams.GetOwningSpec();

	//SourceTags → 施法者身上的标签集合
	//TargetTags → 目标身上的标签集合
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags=EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags=EffectSpec.CapturedTargetTags.GetAggregatedTags();
	
	//存储属性值
	float SourceAttackPower=0.f;
	//AttemptCalculateCapturedAttributeMagnitude用于捕获属性集中属性并存储
	//尝试取出属性值并赋给存储值
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().AttackPowerDef, EvaluateParameters,SourceAttackPower);
	//Debug::Print(TEXT("SourceAttackPower"),SourceAttackPower);

	float BaseDamage=0.f;
	int32 UsedLightAttackComboCount=0;
	int32 UsedHeavyAttackComboCount=0;
	//SetByCaller→是一种动态参数传递方式，调用时由 Ability/代码来设置，动态变化的值用此方法存储->蓝图实现动态赋值
	for (const TPair<FGameplayTag,float>& TagMagnitude:EffectSpec.SetByCallerTagMagnitudes)
	{
		//在GA函数中SetByCallerTagMagnitudes绑定Tag与GA，GA蓝图中调用进行了赋值
		if (TagMagnitude.Key.MatchesTagExact(WarriorGamePlayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage=TagMagnitude.Value;
			//Debug::Print(TEXT("BaseDamage"),BaseDamage);
		}
		if (TagMagnitude.Key.MatchesTagExact(WarriorGamePlayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedLightAttackComboCount=TagMagnitude.Value;
			//Debug::Print(TEXT("UsedLightAttackComboCount"),UsedLightAttackComboCount);
		}
		if (TagMagnitude.Key.MatchesTagExact(WarriorGamePlayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedHeavyAttackComboCount=TagMagnitude.Value;
			//Debug::Print(TEXT("UsedHeavyAttackComboCount"),UsedHeavyAttackComboCount);
		}

	}
	
	float TargetDefensePower=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().DefensePowerDef, EvaluateParameters,TargetDefensePower);
	//Debug::Print(TEXT("TargetDefensePower"),TargetDefensePower);


	/*处理动态获得的属性 */

	//轻击段数基础上的伤害系数增比
	if (UsedLightAttackComboCount != 0)
	{
		const float DamageIncreasePercentLight=(UsedLightAttackComboCount -1)*0.05+1.f;
		BaseDamage*=DamageIncreasePercentLight;
		//Debug::Print(TEXT("ScaledBaseDamageLight"),BaseDamage);
	}
	//重击段数基础上的伤害系数增比
	if (UsedHeavyAttackComboCount!=0)
	{
		const float DamageIncreasePercentHeavy=UsedHeavyAttackComboCount*0.15f+1.f;
		BaseDamage*=DamageIncreasePercentHeavy;
		//Debug::Print(TEXT("ScaledBaseDamageHeavy"),BaseDamage);
	
	}
	//最终伤害判定公式
	const float FinalDamageDone=BaseDamage * SourceAttackPower/TargetDefensePower;
	//Debug::Print(TEXT("FinalDamageDone"),FinalDamageDone);

	//将这个值赋予属性集
	if (FinalDamageDone >0.f)
	{
		//DamageDone覆盖DamageTaken
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(GetWarriorDamageCapture().DamageTakenProperty,EGameplayModOp::Override,FinalDamageDone));
	}
}
