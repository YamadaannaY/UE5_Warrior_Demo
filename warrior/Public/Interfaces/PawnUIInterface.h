// Yu

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnUIInterface.generated.h"

class UEnemyUIComponent;
class UHeroUIComponent;
class UPawnUIComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPawnUIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WARRIOR_API IPawnUIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	// 纯粹的接口函数，实现由其子类承担，其本身只需要给出接口，不需要考虑内容。
	
	//此UI接口被WarriorBaseCharacter所继承。
public:
	//框架级别的统一入口，保证所有实现接口的类都有一个基础 UI。
	virtual UPawnUIComponent* GetPawnUIComponent() const =0;

	//特定角色下获取UIComponent的实现。
	virtual UHeroUIComponent* GetHeroUIComponent() const;
	
	//特定角色下获取UIComponent的实现。
	virtual UEnemyUIComponent* GetEnemyUIComponent() const;
};
