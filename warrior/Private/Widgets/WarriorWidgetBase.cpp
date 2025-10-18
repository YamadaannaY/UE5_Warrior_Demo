// Yu


#include "Widgets/WarriorWidgetBase.h"

#include "Components/UI/HeroUIComponent.h"
#include "Interfaces/PawnUIInterface.h"

//由于HeroUI是一定存在的，所以在引擎第一次初始化会调用，但是EnemyUI要等到对应的Enemy生成时产生，不要在第一次初始化就创建
//而是在EnemyCharacter的BeginPlay()中创建。
void UWarriorWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	//获得UI接口。
	if (const IPawnUIInterface* PawnUIInterface=Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		//使用重写的接口函数获得HeroUIComponent()
		if (UHeroUIComponent* HeroUIComponent=PawnUIInterface->GetHeroUIComponent())
		{
			BP_OnOwingHeroUIComponentInitialized(HeroUIComponent);
		}
	}
}

//在EnemyCharacter的BeginPlay()中调用
void UWarriorWidgetBase::InitEnemyCreatedWidget(AActor* OwingEnemyActor)
{
	if (const IPawnUIInterface* PawnUIInterface=Cast<IPawnUIInterface>(OwingEnemyActor))
	{
		UEnemyUIComponent* EnemyUIComponent=PawnUIInterface->GetEnemyUIComponent();
		
		checkf(EnemyUIComponent,TEXT("failed to extract an EnemyUIComponent from %s "),*OwingEnemyActor->GetActorNameOrLabel())
		BP_OnOwingEnemyUIComponentInitialized(EnemyUIComponent);
	}

}
