// Yu


#include "GameMode/WarriorSurvialGameMode.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Engine/AssetManager.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "WarriorFunctionLibrary.h"

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
			//CurrentWavesCount++;
			
			//判断是否完成所有波次，是则AllWaveDone，否则WaitSpawnNewWave，进入下一波次循环
			if (HasFinishedAllWaves())
			{
				SetCurrentGameModeState(EWarriorSurvivalGameModeState::AllWaveDone);
			}
			else
			{	CurrentWavesCount++;
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
	return CurrentWavesCount>=TotalWavesToSpawn;
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
			//随机找一个匹配TargetPoints
			const int32 RandomTargetPointIndex=FMath::RandRange(0,TargetPointsArray.Num()-1);
			const FVector SpawnOrigin=TargetPointsArray[RandomTargetPointIndex]->GetActorLocation();
			
			//只返回TargetPoint前向向量的纯方向旋转，其他两个分量取默认值
			const FRotator SpawnRotation=TargetPointsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();

			//将匹配TargetPoints的位置随机化，同时抬高z轴避免陷入地面
			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this,SpawnOrigin,RandomLocation,400.f);
			RandomLocation+=FVector(0.f,0.f,150.f);
			
			//根据位置和旋转、具体的UClass对象生成Enemy类
			AWarriorEnemyCharacter* SpawnEnemy=GetWorld()->SpawnActor<AWarriorEnemyCharacter>(LoadedEnemyClass,RandomLocation,SpawnRotation,SpawnParam);

			//当Enemy生成时绑定回调，
			if (SpawnEnemy)
			{
				SpawnEnemy->OnDestroyed.AddUniqueDynamic(this,&ThisClass::OnEnemyDestroyed);
				//记录当前生成的Enemy个数
				EnemiesSpawnedThisTime++;
				//判断波次完成
				TotalSpawnEnemiesThisWaveCounter++;
			}
			//如果达到当前波次个数，提前退出循环避免多生成
			if (!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}
		}
	}
	return EnemiesSpawnedThisTime;;
}

//在OnEnemyDestroyed中用于判断，在单次生成没有达到关卡要求个数时，敌方单位被击杀就生成新的，直到满足总个数
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
			//增加计数，使得波次不进入Completed，同时赋予其减少计数的能力，将其作为Completed的标准
			CurrentSpawnEnemiesCounter++;
			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this,&ThisClass::OnEnemyDestroyed);
		}
	}
}
