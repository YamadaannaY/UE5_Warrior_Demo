// Yu


#include "Widgets/WarriorWidgetBase.h"

#include "Components/UI/HeroUIComponent.h"
#include "Interfaces/PawnUIInterface.h"

void UWarriorWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	//获得pawn类的父抽象接口。
	if (const IPawnUIInterface* PawnUIInterface=Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		//使用具体接口函数获得HeroUIComponent()
		if (UHeroUIComponent* HeroUIComponent=PawnUIInterface->GetHeroUIComponent())
		{
			//
			BP_OnOwingHeroUIComponentInitialized(HeroUIComponent);
		}
	}
}

void UWarriorWidgetBase::InitEnemyCreatedWidget(AActor* OwingEnemyActor)
{
	if (const IPawnUIInterface* PawnUIInterface=Cast<IPawnUIInterface>(OwingEnemyActor))
	{
		UEnemyUIComponent* EnemyUIComponent=PawnUIInterface->GetEnemyUIComponent();
		
		checkf(EnemyUIComponent,TEXT("failed to extract an EnemyUIComponent from %s "),*OwingEnemyActor->GetActorNameOrLabel())
		BP_OnOwingEnemyUIComponentInitialized(EnemyUIComponent);
	}

}
