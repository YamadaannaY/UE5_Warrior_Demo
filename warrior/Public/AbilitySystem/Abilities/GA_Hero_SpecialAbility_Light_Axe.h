// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GA_Hero_SpecialAbilityBase.h"
#include "GA_Hero_SpecialAbility_Light_Axe.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UGA_Hero_SpecialAbility_Light_Axe : public UGA_Hero_SpecialAbilityBase
{
	GENERATED_BODY()
private:
	virtual void HandleEventReceived(FGameplayEventData InPayLoad) override;
	
	FGameplayCueParameters MakeBlockGamePlayCueParams() const ;
	
	void ExecuteGameplayCueToOwnerWithParams(const FGameplayTag InGameplayTag) const;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag GameplayCueTag;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly)
	int8 UseComboCount;
};
