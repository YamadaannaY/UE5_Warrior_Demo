// Yu

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"


UCLASS()
class WARRIOR_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()

protected:
	//获得OwingPawn
	template<class T>
	T* GetOwingPawn() const
	{
		//assert是在运行时检查，而static_在编译阶段检查是否成立，不成立就报错
		static_assert(TPointerIsConvertibleFromTo<T,APawn>::Value,"'T' Template Parameter to GetPawn must be derived from APawn");
		return CastChecked<T>(GetOwner());
	}
	
	//获得当前Owner并将这个Owner转换为pawn类型，可用于玩家角色和敌方角色（模板实例化）。
	APawn* GetMyOwningPawn() const
	{
		return GetOwingPawn<APawn>();
	}

	//获得当前Controller
	template<class T>
	T* GetOwingController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T,AController>::Value,"'T' Template Parameter to etController must be derived from AController");
		return GetOwingPawn<APawn>()->GetController<T>();
	}
};
