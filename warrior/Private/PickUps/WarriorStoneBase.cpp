// Yu


#include "PickUps/WarriorStoneBase.h"
#include "WarriorGamePlayTags.h"
#include "Characters/WarriorHeroCharacter.h"

void AWarriorStoneBase::Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel)
{
	check(StoneGameplayEffectClass);

	//CDO:所有UCLASS都拥有的默认模板对象，包括所有在此类中被定义并且在编辑器中被设置的值，也就是我们需要的Stone设置中的GESubclass
	const UGameplayEffect* EffectCDO=StoneGameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	//使用CDO可以不实例化GE，节省开销
	AbilitySystemComponent->ApplyGameplayEffectToSelf(EffectCDO,ApplyLevel,AbilitySystemComponent->MakeEffectContext());

	BP_OnStoneConsumed();
	
}

void AWarriorStoneBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                            AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                            const FHitResult& SweepResult)
{
	if (const AWarriorHeroCharacter* OverlappedHeroCharacter=Cast<AWarriorHeroCharacter>(OtherActor))
	{
		//激活GA，在GA生效期间收集所有Stone并用数组存储，当产生输入映射后对所有Stone调用Consume
		OverlappedHeroCharacter->GetWarriorAbilitySystemComponent()->TryActivateAbilityByTag(WarriorGamePlayTags::Player_Ability_PickUp_Stone);
	}
}
