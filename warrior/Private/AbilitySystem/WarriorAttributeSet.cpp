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
	InitMaxHealth(1.f);
	InitCurrentRage(1.f);
	InitMaxRage(1.f);
	InitAttackPower(1.f);
	InitDefensePower(1.f);
}


void UWarriorAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface=TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}
	checkf(CachedPawnUIInterface.IsValid(),TEXT("%s did`t implement IPawnUIInterface "),*Data.Target.GetAvatarActor()->GetActorNameOrLabel());
	UPawnUIComponent* PawnUIComponent=CachedPawnUIInterface->GetPawnUIComponent();
	checkf(PawnUIComponent,TEXT("could`t extract a PawnUIComponent from %s "),*Data.Target.GetAvatarActor()->GetActorNameOrLabel());
	if(Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth=FMath::Clamp(GetCurrentHealth(),0.f,GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth()/GetMaxHealth());
	}
	if (Data.EvaluatedData.Attribute==GetCurrentRageAttribute())
	{
		const float NewCurrentRage=FMath::Clamp(GetCurrentRage(),0.f,GetMaxRage());
		SetCurrentRage(NewCurrentRage);
		if (UHeroUIComponent*HeroUIComponent=CachedPawnUIInterface->GetHeroUIComponent())
		{
			HeroUIComponent->OnCurrentRageChanged.Broadcast(GetCurrentRage()/GetMaxRage());
		}
		
	}
	if (Data.EvaluatedData.Attribute==GetDamageTakenAttribute())
	{
		const float OldHealth=GetCurrentHealth();
		const float DamageDone=GetDamageTaken();
		const float NewCurrentHealth=FMath::Clamp(OldHealth-DamageDone,0.f,GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);
		const FString DebugString=FString::Printf(TEXT("Old Health:%f,Damage Done:%f,New Current Health:%f"),OldHealth,DamageDone,NewCurrentHealth);
		Debug::Print(DebugString,FColor::Green);
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth()/GetMaxHealth());

		//death
		if (GetCurrentHealth()==0.f)
		{	//将tag传给Data.Target.GetAvatarActor()
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(),WarriorGamePlayTags::Shared_Status_Dead);
		}
	}
}
