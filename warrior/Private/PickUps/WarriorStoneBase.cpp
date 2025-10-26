// Yu


#include "PickUps/WarriorStoneBase.h"

#include "WarriorGamePlayTags.h"
#include "Characters/WarriorHeroCharacter.h"

void AWarriorStoneBase::Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel)
{
	check(StoneGameplayEffectClass);

	//所有UCLASS都拥有的默认模板对象，包括所有在类中被定义并且在编辑器中国被设置的值，也就是我们需要的GE对象
	UGameplayEffect* EffectCDO=StoneGameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	
	AbilitySystemComponent->ApplyGameplayEffectToSelf(EffectCDO,ApplyLevel,AbilitySystemComponent->MakeEffectContext());

	BP_OnStoneConsumed();
	
}

void AWarriorStoneBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                            AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                            const FHitResult& SweepResult)
{
	if (AWarriorHeroCharacter* OverlappedHeroCharacter=Cast<AWarriorHeroCharacter>(OtherActor))
	{
		//激活GA，在GA生效期间收集所有Stone并用数组存储，当输入映射后应用所有Stone的GE
		OverlappedHeroCharacter->GetWarriorAbilitySystemComponent()->TryActivateAbilityByTag(WarriorGamePlayTags::Player_Ability_PickUp_Stone);
	}
}
