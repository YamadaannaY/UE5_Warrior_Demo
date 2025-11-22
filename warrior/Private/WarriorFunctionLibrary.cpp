// Yu
/** Blueprint Function Library **/

#include "WarriorFunctionLibrary.h"
#include "abilitysystem/WarriorAbilitySystemComponent.h"
#include "Characters/WarriorBaseCharacter.h"
#include "Conrtroller/WarriorHeroController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectTypes.h"
#include "WarriorGameInstance.h"
#include "WarriorGamePlayTags.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/WarriorSaveGame.h"
#include "WarriorTypes/WarriorCountDownAction.h"


UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	check(InActor)
	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC=NativeGetWarriorASCFromActor(InActor);

	if (!ASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddGameplayTagToActorIfNone: Failed to get ASC from actor %s"), 
			*InActor->GetName());
		return;
	}
	
	if (! ASC->HasMatchingGameplayTag(TagToAdd))
	{
		//不依赖 GE，完全由开发者代码控制的Tag，根据计数逻辑进行添加或删除
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UWarriorAbilitySystemComponent* ASC=NativeGetWarriorASCFromActor(InActor);
	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		//完全由开发者代码控制的Tag，开发者具有完全自主权。引用计数逻辑
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarriorAbilitySystemComponent* ASC=NativeGetWarriorASCFromActor(InActor);
	return ASC->HasMatchingGameplayTag(TagToCheck);
}


void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,
	EWarriorConfirmType& OutConfirmType)
{
	//输出引脚
	OutConfirmType=NativeDoesActorHaveTag(InActor,TagToCheck) ? EWarriorConfirmType::Yes : EWarriorConfirmType::No;
}

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);
	//获得接口，以接口函数访问CombatComponent
	if (const IPawnCombatInterface* PawnCombatInterface=Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}
	return nullptr;
}

UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,
	EWarriorValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent=NativeGetPawnCombatComponentFromActor(InActor);
	OutValidType=CombatComponent ? EWarriorValidType::Valid : EWarriorValidType::Invalid;
	return CombatComponent;
}

bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);
	//通过Controller的接口获得TeamId
	const IGenericTeamAgentInterface* QueryTeamAgent=Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	const IGenericTeamAgentInterface* TargetTeamAgent=Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent && TargetTeamAgent)
	{
		//Hostile
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}
	return false;
}

bool UWarriorFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);
	//获得二者前向向量弧度值。从相对的-1到垂直的0都可以判定为成功阻挡
	const float DotResult=FVector::DotProduct(InAttacker->GetActorForwardVector(),InDefender->GetActorForwardVector());
	/*const FString DebugString=FString::Printf(TEXT("Dot Result:%f %s"),DotResult,DotResult<-0.1f ? TEXT("Valid Block") : TEXT("InValid Block"));
	Debug::Print(DebugString,DotResult<-0.1f ? FColor::Green : FColor::Red);*/
	
	//0.1误差值，小于零即二者相对成立，格挡成功
	return DotResult<-0.1f;
}

bool UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	UWarriorAbilitySystemComponent* SourceASC=NativeGetWarriorASCFromActor(InInstigator);
	UWarriorAbilitySystemComponent* TargetASC=NativeGetWarriorASCFromActor(InTargetActor);

	//将传入GESpecHandle应用到目标上，执行GE逻辑。
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle=
		SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data,TargetASC);
 
	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

void UWarriorFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval,
	float& OutRemainingTime, EWarriorCountDownActionInput CountDownInput,
	UPARAM(DisplayName="Output")EWarriorCountDownActionOutput& CountDownOutput, FLatentActionInfo LatenInfo)
{
	UWorld* World=nullptr;
	
	//获得传入WorldContextObject对应的World
	if (GEngine)
	{
		World=GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull);
	}

	if (!World)
	{
		return;
	}
	//FLatentActionManager是引擎中管理所有延迟操作的全局管理器。每个UWorld都有一个。它的作用是存储、更新和清理所有正在进行的延迟操作。
	FLatentActionManager& LatenActionManager=World->GetLatentActionManager();

	//LatentInfo.CallbackTarget：通常是调用这个延迟函数的对象，用于确定操作的作用域。
	//LatentInfo.UUID：一个唯一标识符，用于区分同一对象上的不同延迟操作Action。
	FWarriorCountDownAction* FoundAction=LatenActionManager.FindExistingAction<FWarriorCountDownAction>(LatenInfo.CallbackTarget,LatenInfo.UUID);

	//如果输入引脚是Start
	if (CountDownInput==EWarriorCountDownActionInput::Start)
	{
		CountDownOutput = EWarriorCountDownActionOutput::Updated; // 用于将上一次冷却后Output的Completed状态重置为初始状态
		if (!FoundAction)
		{
			//没有创建好的延迟Action类，则new一个特定的延迟操作类
			LatenActionManager.AddNewAction(LatenInfo.CallbackTarget,LatenInfo.UUID,new FWarriorCountDownAction(TotalTime,UpdateInterval,OutRemainingTime,CountDownOutput,LatenInfo));
		}
	}
	//如果输入引脚是Cancel，则取消这个Action，用于卸下武器时同步移除倒计时和技能图标
	if (CountDownInput==EWarriorCountDownActionInput::Cancel)
	{
		if (FoundAction)
		{
			FoundAction->CancelAction();
		}
	}
}

UWarriorGameInstance* UWarriorFunctionLibrary::GetWarriorGameInstance(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		if (UWorld* World=GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull))
		{
			return World->GetGameInstance<UWarriorGameInstance>();
		}
	}
	return nullptr;
}

void UWarriorFunctionLibrary::ToggleInputMode(const UObject* WorldContextObject, EWarriorInputMode InInputMode)
{
	APlayerController* PlayerController=nullptr;

	if (GEngine)
	{
		if (UWorld* World=GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull))
		{
			PlayerController=World->GetFirstPlayerController();
		}
	}
	
	if (!PlayerController)
	{
		return ;
	}
	
	FInputModeGameOnly GameOnlyMode;
	FInputModeUIOnly UIOnlyMode;
	
	switch (InInputMode)
	{
	case EWarriorInputMode::GameOnly:
		PlayerController->SetInputMode(GameOnlyMode);
		PlayerController->bShowMouseCursor=false;
		break;
	case EWarriorInputMode::UIOnly:
		PlayerController->SetInputMode(UIOnlyMode);
		PlayerController->bShowMouseCursor=true;
		break;
	default:
		break;
	}
}

void UWarriorFunctionLibrary::SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficultyToSave)
{
	//CreateSaveGameObject返回基类指针，需要进行一次手动Cast
	USaveGame* SaveGameObject=UGameplayStatics::CreateSaveGameObject(UWarriorSaveGame::StaticClass());

	if (UWarriorSaveGame* WarriorSaveGameObject=Cast<UWarriorSaveGame>(SaveGameObject))
	{
		WarriorSaveGameObject->SavedCurrentDifficulty=InDifficultyToSave;

		//用Tag保持固定的Slot格式，便于存储遍历
		UGameplayStatics::SaveGameToSlot(WarriorSaveGameObject,WarriorGamePlayTags::GameData_SaveGame_Slot_1.GetTag().ToString(),0);
		/*Debug::Print(bWasSaved ? TEXT("DifficultySaved") : TEXT("DifficultyNotSaved"));*/
	}
}

bool UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(EWarriorGameDifficulty& OutSaveDifficulty)
{
	if (UGameplayStatics::DoesSaveGameExist(WarriorGamePlayTags::GameData_SaveGame_Slot_1.GetTag().ToString(),0))
	{
		USaveGame* SaveGameObject=UGameplayStatics::LoadGameFromSlot(WarriorGamePlayTags::GameData_SaveGame_Slot_1.GetTag().ToString(),0);

		if (const UWarriorSaveGame* WarriorSaveGameObject=Cast<UWarriorSaveGame>(SaveGameObject))
		{
			OutSaveDifficulty=WarriorSaveGameObject->SavedCurrentDifficulty;
			/*Debug::Print(TEXT("Loading Successfully"),FColor::Green);*/
			return true;
		}
	}
	return false;
}


