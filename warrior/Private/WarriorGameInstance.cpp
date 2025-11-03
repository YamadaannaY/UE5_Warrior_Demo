// Yu

#include "WarriorGameInstance.h"
#include "MoviePlayer.h"
#include "Characters/WarriorEnemyCharacter.h"

void UWarriorGameInstance::Init()
{
	Super::Init();
	//绑定委托，在预加载Map之后执行回调
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this,&ThisClass::OnPreLoadMap);
}

void UWarriorGameInstance::OnPreLoadMap(const FString& MapName)
{
	//加载屏幕显示参数
	FLoadingScreenAttributes LoadingScreenAttributes;
	//当加载完毕后自动完成，至少加载2s，最后是一个自带默认的加载Widget，具有基础进度条
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes=true;
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime=2.f;
	LoadingScreenAttributes.WidgetLoadingScreen=FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	//将这个屏幕参数设置为加载屏幕
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
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
