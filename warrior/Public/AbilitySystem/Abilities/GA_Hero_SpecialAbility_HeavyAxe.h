// Yu

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GA_Hero_SpecialAbilityBase.h"
#include "GA_Hero_SpecialAbility_HeavyAxe.generated.h"

/**
 * 
 */
UCLASS()
class WARRIOR_API UGA_Hero_SpecialAbility_HeavyAxe : public UGA_Hero_SpecialAbilityBase
{
	GENERATED_BODY()

private:
	virtual void HandleEventReceived(FGameplayEventData InPayLoad) override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly)
	int8 HeavyUseComboCount;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypeQuery;

	UPROPERTY(EditDefaultsOnly)
	FVector BoxSize=FVector(500.f,500.f,200.f);

	UPROPERTY(EditDefaultsOnly)
	bool bDrawDebugShape;
	
};
