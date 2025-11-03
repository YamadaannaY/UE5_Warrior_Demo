// Yu

#pragma once

#include "CoreMinimal.h"
#include "GameMode/WarriorGameMode.h"
#include "WarriorSurvialGameMode.generated.h"

class AWarriorEnemyCharacter;

UENUM(BlueprintType)
enum class EWarriorSurvivalGameModeState:uint8
{
	WaitSpawnNewWave,
	SpawningNewWave,
	InProgress,
	WaveCompleted,
	AllWaveDone,
	PlayerDied,
};

USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerInfo
{
	GENERATED_BODY()

	//软引用,在新波次之前异步加载
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn;

	//生成EnemyClass的最小数量
	UPROPERTY(EditAnywhere)
	int8 MinPerSpawnCount=1;
	
	//生成EnemyClass的最大数量
	UPROPERTY(EditAnywhere)
	int8 MaxPerSpawnCount=3;
};

//实现Mode DT的内容行
USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerTableRow:	public FTableRowBase
{
	GENERATED_BODY()

	//存储单波次内的所有EnemyClass类型及其配置信息
	UPROPERTY(EditAnywhere)
	TArray<FWarriorEnemyWaveSpawnerInfo> EnemyWaveSpawnerDefinitions;

	//此波次内一共要生成多少Enemy
	UPROPERTY(EditAnywhere)
	int32 TotalEnemyToSpawnThisWave=1;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurvivalGameModeStateChanged,EWarriorSurvivalGameModeState,CurrentState);

/**
 * 
 */

UCLASS()
class WARRIOR_API AWarriorSurvivalGameMode : public AWarriorGameMode
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);


private:

	/**内部使用函数 **/
	
	//设置当前State
	void SetCurrentGameModeState(EWarriorSurvivalGameModeState InState);
	
	//判断是否所有波次结束
	bool HasFinishedAllWaves() const ;
	
	//异步加载下一波次的敌方单位
	void PreLoadNextWaveSpawnEnemies();
	
	//通过RowName检索当前波次对应的DT行，获取具体配置信息
	FWarriorEnemyWaveSpawnerTableRow* GetCurrentWaveSpawnerTableRow() const;
	
	//将加载好的敌方单位在指定位置进行生成
	int32 TrySpawnWaveEnemies();
	
	//是否继续生成，判断是否达到波次生成地方单位数量要求
	bool ShouldKeepSpawnEnemies() const;
	
	//用于在当前波次内敌人全部被消灭且此波次要生成的总敌方单位数还未达标时调用
	UFUNCTION()
	void OnEnemyDestroyed(AActor* DestroyedActor);
	/**内部使用函数 **/
	
	//当前State
	UPROPERTY()
	EWarriorSurvivalGameModeState CurrentSurvivalGameModeState;

	//蓝图绑定委托，此时广播对应改变的State需要进行的逻辑将回调执行
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnSurvivalGameModeStateChanged OnSurvivalGameModeStateChanged;

	//DT对象，在DT实例中配置各波次的EnemyClass、TotalCount等等
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WaveDefinition",meta=(AllowPrivateAccess=true))
	UDataTable* EnemyWaveSpawnerDataTable;

	//总波次
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="WaveDefinition",meta=(AllowPrivateAccess=true))
	int32 TotalWavesToSpawn;

	//当前波次计数
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="WaveDefinition",meta=(AllowPrivateAccess=true))
	int32 CurrentWavesCount=1;

	//当前存在的敌方单位个数
	UPROPERTY()
	int32 CurrentSpawnEnemiesCounter=0;

	//此波次总共生成的敌方单位
	UPROPERTY()
	int32 TotalSpawnEnemiesThisWaveCounter=0;

	//存储了所有的TargetPoint Actor,在其位置上进行敌方单位生成
	UPROPERTY()
	TArray<AActor*> TargetPointsArray;

	//判断间隔时间
	UPROPERTY()
	float TimePassedSinceStart=0.f;

	//等待时间之后波次间隔时间，有UI倒计时
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WaveDefinition",meta=(AllowPrivateAccess=true))
	float SpawnNewWavesWaitTime=5.f;

	//波次开始后生成Enemy的间隔时间
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="WaveDefinition",meta=(AllowPrivateAccess=true))
	float SpawnEnemiesDelayTime=2.f;

	//一波次之后的间隔时间，用来预加载EnemyClass，添加波次计数，给玩家进行缓冲
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="WaveDefinition",meta=(AllowPrivateAccess=true))
	float WaveCompletedWaitTime=5.f;

	//存储加载的角色和其软引用的映射Map
	UPROPERTY()
	TMap<TSoftClassPtr<AWarriorEnemyCharacter>,UClass*> PreLoadedEnemyClassMap;

public:
	//将Boss生成的敌方单位个数加入TotalSpawnEnemiesThisWaveCounter，赋予其调用OnEnemyDestroyed的能力
	UFUNCTION(BlueprintCallable)
	void RegisterSpawnedEnemies(const TArray<AWarriorEnemyCharacter*>& InEnemiesToRegister);
	
};
