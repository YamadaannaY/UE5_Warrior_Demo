// Yu


#include "AbilitySystem/WarriorAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorDebugHelper.h"
#include "WarriorGamePlayTags.h"
#include "Components/UI/HeroUIComponent.h"
#include "Components/UI/PawnUIComponent.h"
#include "Interfaces/PawnUIInterface.h"

//构造函数中使用访问器函数进行所有值的初始化，在编辑器中使用CurveTable进行赋值
UWarriorAttributeSet::UWarriorAttributeSet()
{
	InitCurrentHealth(1.f);
	InitCurrentRage(1.f);
	InitMaxHealth(1.f);
	InitMaxRage(1.f);
	InitAttackPower(1.f);
	InitDefensePower(1.f);
}

void UWarriorAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	// FGameplayEffectModCallbackData：包含属性修改的上下文：被修改的目标 发起修改的源 整个GameplayEffect的上下文  本次执行的最终计算结果
	if (!CachedPawnUIInterface.IsValid())
	{
		//第一次调用时，创建一个弱引用的UIInterface
		CachedPawnUIInterface=TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}
	checkf(CachedPawnUIInterface.IsValid(),TEXT("%s did`t implement IPawnUIInterface "),*Data.Target.GetAvatarActor()->GetActorNameOrLabel());

	//通过接口获得UIComponent
	const UPawnUIComponent* PawnUIComponent=CachedPawnUIInterface->GetPawnUIComponent();
	checkf(PawnUIComponent,TEXT("could`t extract a PawnUIComponent from %s "),*Data.Target.GetAvatarActor()->GetActorNameOrLabel());

	/**被修改的属性判断 **/
	if(Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		//获得经过Clamp限制后的值
		const float NewCurrentHealth=FMath::Clamp(GetCurrentHealth(),0.f,GetMaxHealth());

		//限制后的值进行实际赋予
		SetCurrentHealth(NewCurrentHealth);
		
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth()/GetMaxHealth());
	}
	if (Data.EvaluatedData.Attribute==GetCurrentRageAttribute())
	{
		const float NewCurrentRage=FMath::Clamp(GetCurrentRage(),0.f,GetMaxRage());
		SetCurrentRage(NewCurrentRage);

		if (GetCurrentRage()==GetMaxRage())
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(),WarriorGamePlayTags::Player_Status_Rage_Full);
		}
		else if (GetCurrentRage() ==0.f)
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(),WarriorGamePlayTags::Player_Status_Rage_None);
		}
		else
		{
			//处于中间状态
			UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(),WarriorGamePlayTags::Player_Status_Rage_Full);
			UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(),WarriorGamePlayTags::Player_Status_Rage_None);
		}
		
		if (const UHeroUIComponent*HeroUIComponent=CachedPawnUIInterface->GetHeroUIComponent())
		{
			HeroUIComponent->OnCurrentRageChanged.Broadcast(GetCurrentRage()/GetMaxRage());
		}
	}
	//根据造成的伤害修改当前生命值，附加伤害计算后的死亡逻辑判断
	if (Data.EvaluatedData.Attribute==GetDamageTakenAttribute())
	{
		const float OldHealth=GetCurrentHealth();
		const float DamageDone=GetDamageTaken();
		const float NewCurrentHealth=FMath::Clamp(OldHealth-DamageDone,0.f,GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);
		
		/*const FString DebugString=FString::Printf(TEXT("Old Health:%f,Damage Done:%f,New Current Health:%f"),OldHealth,DamageDone,NewCurrentHealth);
		Debug::Print(DebugString,FColor::Green);*/
		
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth()/GetMaxHealth());

		//Death：触发GA_Dead
		if (GetCurrentHealth()==0.f)
		{	//将Tag添加到Data.Target.GetAvatarActor()即敌方单位上
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone
			(Data.Target.GetAvatarActor(),WarriorGamePlayTags::Shared_Status_Dead);
		}
	}
}
