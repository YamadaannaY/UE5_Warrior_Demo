// Yu

#include "WarriorGameInstance.h"

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
