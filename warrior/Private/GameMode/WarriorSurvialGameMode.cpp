// Yu


#include "GameMode/WarriorSurvialGameMode.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Engine/AssetManager.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "WarriorFunctionLibrary.h"

/** GameMode流程： 在最开始为WaitSpawnNewWave状态，异步加载下一波次对象，然后进入波次间隔时间，此时为SpawningNewWave，再进入敌方单位生成时间，
 *  此时InProgress，进行加载对象的实例化。再是等待时间InProgress，完成所有后进入WaveCompleted，这里有一段缓冲时间，用来加载下一波次敌方单位。		**/

void AWarriorSurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();
	checkf(EnemyWaveSpawnerDataTable,TEXT("Forgot to assign a data table in survival game mode blueprint"));

	//初始状态为等待新波次生成
	SetCurrentGameModeState(EWarriorSurvivalGameModeState::WaitSpawnNewWave);
	//获取Waves数量并进行存储，通过获取DT编辑器中配置Row的数量实现
	TotalWavesToSpawn=EnemyWaveSpawnerDataTable->GetRowNames().Num();
	
	//每次都在WaitSpawnNewWave状态预加载下一波次的Class异步加载下一波次的敌方单位
	PreLoadNextWaveSpawnEnemies();
}

//每一帧都对State进行判断
void AWarriorSurvivalGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//在WaitSpawnNewWave状态下读秒SpawnNewWavesWaitTime
	if (CurrentSurvivalGameModeState==EWarriorSurvivalGameModeState::WaitSpawnNewWave)
	{
		TimePassedSinceStart+=DeltaTime;
		//读秒完毕后，从WaitSpawnNewWave进入SpawningNewWave
		if (TimePassedSinceStart>=SpawnNewWavesWaitTime)
		{
			TimePassedSinceStart=0.f;
			SetCurrentGameModeState(EWarriorSurvivalGameModeState::SpawningNewWave);
		}
	}

	//读秒SpawnEnemiesDelayTime，随后生成预加载好的敌方单位，进入InProgress状态
	if (CurrentSurvivalGameModeState==EWarriorSurvivalGameModeState::SpawningNewWave)
	{
		TimePassedSinceStart+=DeltaTime;

		if (TimePassedSinceStart>=SpawnEnemiesDelayTime)
		{
			CurrentSpawnEnemiesCounter+=TrySpawnWaveEnemies();
			TimePassedSinceStart=0.f;
			SetCurrentGameModeState(EWarriorSurvivalGameModeState::InProgress);
		}
	}
	//当前波次所有EnemyClass生成完毕后，进入WaveCompleted状态，有一段空白时间作为缓冲
	if (CurrentSurvivalGameModeState==EWarriorSurvivalGameModeState::WaveCompleted)
	{
		TimePassedSinceStart+=DeltaTime;
		if (TimePassedSinceStart>=WaveCompletedWaitTime)
		{
			TimePassedSinceStart=0.f;
			CurrentWavesCount++;
			
			//判断是否完成所有波次，是则AllWaveDone，否则WaitSpawnNewWave，进入下一波次循环
			if (HasFinishedAllWaves())
			{
				SetCurrentGameModeState(EWarriorSurvivalGameModeState::AllWaveDone);
			}
			else
			{
				SetCurrentGameModeState(EWarriorSurvivalGameModeState::WaitSpawnNewWave);
				PreLoadNextWaveSpawnEnemies();
			}
		}
	}
}

void AWarriorSurvivalGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	EWarriorGameDifficulty SavedGameDifficulty;
	if (UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(SavedGameDifficulty))
	{
		CurrentGameDifficulty=SavedGameDifficulty;
	}
}

void AWarriorSurvivalGameMode::SetCurrentGameModeState(EWarriorSurvivalGameModeState InState)
{
	CurrentSurvivalGameModeState = InState;
	OnSurvivalGameModeStateChanged.Broadcast(CurrentSurvivalGameModeState);
}

bool AWarriorSurvivalGameMode::HasFinishedAllWaves() const
{
	//这个判断在自增操作之后，所以实际CurrentWavesCount会比真正参与判断的当前波次数大1
	return CurrentWavesCount>TotalWavesToSpawn;
}

void AWarriorSurvivalGameMode::PreLoadNextWaveSpawnEnemies()
{
	if (HasFinishedAllWaves())
	{
		return;
	}
	//每次预加载必须首先保证数组是空的
	PreLoadedEnemyClassMap.Empty();
	
	//遍历当前轮次的EnemyWaveSpawnerDefinitions
	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo:GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;
		
		//采用异步加载并存储加载结果
		UAssetManager::GetStreamableManager().RequestAsyncLoad(SpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),FStreamableDelegate::CreateLambda([SpawnerInfo,this]()
		{
			if (UClass* LoadedEnemyClass=SpawnerInfo.SoftEnemyClassToSpawn.Get())
			{
				PreLoadedEnemyClassMap.Emplace(SpawnerInfo.SoftEnemyClassToSpawn,LoadedEnemyClass);
			}
		}));
	}

	
}


FWarriorEnemyWaveSpawnerTableRow* AWarriorSurvivalGameMode::GetCurrentWaveSpawnerTableRow() const
{
	//固定的Row头名的格式eg：Wave1 etc..
	const FName RowName=FName(TEXT("Wave")+FString::FromInt(CurrentWavesCount));
	//获取Row下数据
	FWarriorEnemyWaveSpawnerTableRow* FoundRow=EnemyWaveSpawnerDataTable->FindRow<FWarriorEnemyWaveSpawnerTableRow>(RowName,FString());

	checkf(FoundRow,TEXT("could not find the valid row under the name %s in the data table "),*RowName.ToString());

	return FoundRow;
}

int32 AWarriorSurvivalGameMode::TrySpawnWaveEnemies()
{
	if (TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this,ATargetPoint::StaticClass(),TargetPointsArray);
	}

	checkf(!TargetPointsArray.IsEmpty(),TEXT("no valid target point found in level:%s for spawning enemies"),*GetWorld()->GetName());

	uint32 EnemiesSpawnedThisTime =0;
 
	//制定了生成对象时的碰撞处理方式参数
	FActorSpawnParameters SpawnParam;
	SpawnParam.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const FWarriorEnemyWaveSpawnerInfo SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull()) continue;
		//此次SpawnEnemy的个数
		const int32 NumToSpawn=FMath::RandRange(SpawnerInfo.MinPerSpawnCount,SpawnerInfo.MaxPerSpawnCount);
		
		//找到对应加载好的UClass对象
		UClass* LoadedEnemyClass=PreLoadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);

		for (int32 i=0;i<NumToSpawn;i++)
		{
			//随机找一个位置
			const int32 RandomTargetPointIndex=FMath::RandRange(0,TargetPointsArray.Num()-1);
			const FVector SpawnOrigin=TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			//只返回TargetPoint前向向量的纯方向旋转，其他两个分量取默认值
			const FRotator SpawnRotation=TargetPointsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();
			//将这个生成位置进一步随机化
			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this,SpawnOrigin,RandomLocation,400.f);
			//将生成位置抬高，避免陷入地面
			RandomLocation+=FVector(0.f,0.f,150.f);
			
			//根据位置和旋转、UClass信息生成Enemy
			AWarriorEnemyCharacter* SpawnEnemy=GetWorld()->SpawnActor<AWarriorEnemyCharacter>(LoadedEnemyClass,RandomLocation,SpawnRotation,SpawnParam);

			//当Enemy被击杀时，回调OnEnemyDestroyed
			if (SpawnEnemy)
			{
				SpawnEnemy->OnDestroyed.AddUniqueDynamic(this,&ThisClass::OnEnemyDestroyed);
				EnemiesSpawnedThisTime++;
				TotalSpawnEnemiesThisWaveCounter++;
			}
			if (!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}
		}
		
	}
	return EnemiesSpawnedThisTime;;
}

bool AWarriorSurvivalGameMode::ShouldKeepSpawnEnemies() const
{
	return TotalSpawnEnemiesThisWaveCounter < GetCurrentWaveSpawnerTableRow()->TotalEnemyToSpawnThisWave;
}

void AWarriorSurvivalGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	CurrentSpawnEnemiesCounter--;

	/*Debug::Print(FString::Printf(TEXT("CurrentSpawnEnemiesCounter=%i,TotalSpawnedEnemiesThisWaveCounter= %i"),CurrentSpawnEnemiesCounter,TotalSpawnEnemiesThisWaveCounter)); */
	if (ShouldKeepSpawnEnemies())
	{
		CurrentSpawnEnemiesCounter+=TrySpawnWaveEnemies();
	}
	
	//如果判断不继续生成敌方单位且当前关卡内敌方单位数量为0，意味着波次完成
	else if (CurrentSpawnEnemiesCounter==0)
	{
		TotalSpawnEnemiesThisWaveCounter=0;
		SetCurrentGameModeState(EWarriorSurvivalGameModeState::WaveCompleted);
	}
}

void AWarriorSurvivalGameMode::RegisterSpawnedEnemies(const TArray<AWarriorEnemyCharacter*>& InEnemiesToRegister)
{
	for (AWarriorEnemyCharacter* SpawnedEnemy : InEnemiesToRegister)
	{
		if (SpawnedEnemy)
		{
			CurrentSpawnEnemiesCounter++;
			
			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this,&ThisClass::OnEnemyDestroyed);
		}
	}
}
