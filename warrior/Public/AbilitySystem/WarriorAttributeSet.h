// Yu

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "WarriorAttributeSet.generated.h"

//生成一套访问器：ATTRIBUTE_ACCESSORS 提供功能
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class IPawnUIInterface;
/**
 * 属性集 
 */
UCLASS()
class WARRIOR_API UWarriorAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UWarriorAttributeSet();

	//内部自动调用的生命周期函数，当一个GameplayEffect修改了某个Attribute（注意是单个）时调用，会把结果写入到UWarriorAttributeSet里的对应属性
	//内置if判断进行对应属性更新
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data) override;
	
	UPROPERTY(BlueprintReadOnly,Category="Attribute|Health",ReplicatedUsing=OnRep_CurrentHealth)
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet,CurrentHealth)

	UPROPERTY(BlueprintReadOnly,Category="Attribute|Health",ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet,MaxHealth)

	UPROPERTY(BlueprintReadOnly,Category="Attribute|Rage",ReplicatedUsing=OnRep_CurrentRage)
	FGameplayAttributeData CurrentRage;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet,CurrentRage)

	UPROPERTY(BlueprintReadOnly,Category="Attribute|Rage",ReplicatedUsing=OnRep_MaxRage)
	FGameplayAttributeData MaxRage;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, MaxRage)

	UPROPERTY(BlueprintReadOnly,Category="Attribute|Damage",ReplicatedUsing=OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet,AttackPower)

	UPROPERTY(BlueprintReadOnly,Category="Attribute|Damage",ReplicatedUsing=OnRep_DefensePower)
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet,DefensePower)
	
	UPROPERTY(BlueprintReadOnly,Category="Attribute|Damage",ReplicatedUsing=OnRep_DamageTaken)
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet,DamageTaken)

	UPROPERTY(BlueprintReadOnly,Category="Attribute|Stamina",ReplicatedUsing=OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet,Stamina)

	UPROPERTY(BlueprintReadOnly,Category="Attribute|Stamina",ReplicatedUsing=OnRep_Stamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet,MaxStamina)


private:
	TWeakInterfacePtr<IPawnUIInterface> CachedPawnUIInterface;

public:
	UFUNCTION()
	void OnRep_CurrentHealth(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWarriorAttributeSet,CurrentHealth,OldValue);
	}

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWarriorAttributeSet,MaxHealth,OldValue);
	}

	UFUNCTION()
	void OnRep_CurrentRage(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWarriorAttributeSet,CurrentRage,OldValue);
	}

	UFUNCTION()
	void OnRep_MaxRage(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWarriorAttributeSet,MaxRage,OldValue);
	}

	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWarriorAttributeSet,AttackPower,OldValue);
	}

	UFUNCTION()
	void OnRep_DefensePower(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWarriorAttributeSet,DefensePower,OldValue);
	}

	UFUNCTION()
	void OnRep_DamageTaken(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWarriorAttributeSet,DamageTaken,OldValue);
	}

	UFUNCTION()
	void OnRep_Stamina(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWarriorAttributeSet,Stamina,OldValue);
	}

	UFUNCTION()
	void OnRep_MaxStamina(const FGameplayAttributeData& OldValue) const
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UWarriorAttributeSet,MaxStamina,OldValue);
	}

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
