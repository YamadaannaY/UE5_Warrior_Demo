// Yu

#include "WarriorGameInstance.h"
#include "MoviePlayer.h"
#include "Characters/WarriorEnemyCharacter.h"

void UWarriorGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this,&ThisClass::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this,&UWarriorGameInstance::OnDestinationWorldLoaded);
}

void UWarriorGameInstance::OnPreLoadMap(const FString& MapName)
{
	FLoadingScreenAttributes LoadingScreenAttributes;
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes=true;
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime=2.f;
	LoadingScreenAttributes.WidgetLoadingScreen=FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

void UWarriorGameInstance::OnDestinationWorldLoaded(UWorld* LoadedWorld)
{
	GetMoviePlayer()->StopMovie();
}

TSoftObjectPtr<UWorld> UWarriorGameInstance::GetGameLevelByTag(FGameplayTag InTag) const 
{
	//遍历Set，找到Tag对应的Level并返回
	for (const FWarriorGameLevelSet& GameLevelSet: GameLevelSets)
	{  
		if (!GameLevelSet.IsValid()) continue;
		
		if (GameLevelSet.LevelTag==InTag)
		{
			return GameLevelSet.Level;
		}
	}
	//返回一个空的、无效的软对象指针，比返回nullptr更安全且语义更清晰的方式
	return TSoftObjectPtr<UWorld>();
}
