// Yu


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemy.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Engine/AssetManager.h"
#include "NavigationSystem.h"

UAbilityTask_WaitSpawnEnemy* UAbilityTask_WaitSpawnEnemy::WaitSpawnEnemy(UGameplayAbility* OwningAbility,
                                                                         FGameplayTag EventTag, TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn, int32 NumToSpawn,
                                                                         const FVector& SpawnOrigin, float RandomSpawnRadius, const FRotator& SpawnRotation)
{
	//实例化一个Task蓝图节点，传入形参并用缓存值记录
	UAbilityTask_WaitSpawnEnemy* Node=NewAbilityTask<UAbilityTask_WaitSpawnEnemy>(OwningAbility);
	Node->CachedEventTag=EventTag;
	Node->CachedNumToSpawn=NumToSpawn;
	Node->CachedSoftEnemyClassToSpawn=SoftEnemyClassToSpawn;
	Node->CachedSpawnOrigin=SpawnOrigin;
	Node->CachedRandomSpawnRadius=RandomSpawnRadius;
	Node->CachedSpawnRotation=SpawnRotation; 

	return Node;
}

void UAbilityTask_WaitSpawnEnemy::Activate()
{
	// 1. 找到或创建与指定Tag关联的事件委托
	FGameplayEventMulticastDelegate& Delegate=AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);

	// 2. 注册事件委托回调函数，同时保存这个注册委托句柄，用于后续取消注册
	DelegateHandle=Delegate.AddUObject(this,&ThisClass::OnGameplayEventReceived);
}


void UAbilityTask_WaitSpawnEnemy::OnDestroy(bool bInOwnerFinished)
{
	// 1. 找到之前注册的委托，从中删除注册委托句柄
	FGameplayEventMulticastDelegate& Delegate=AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	Delegate.Remove(DelegateHandle);
	//要求最后调用，处理摧毁的逻辑
	Super::OnDestroy(bInOwnerFinished);
}


void UAbilityTask_WaitSpawnEnemy::OnGameplayEventReceived(const FGameplayEventData* InPayLoad)
{
	//明确传入的Class时执行
	if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
	{
		//异步加载Class，由于逻辑多不用Lambda而是一个绑定了异步委托的回调，每次异步加载调用
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this,&ThisClass::OnEnemyClassLoaded));
	}
	//没有明确要生成的Class
	else
	{
		//需要判断Task是否还在被激活，如果是，广播一个空数组
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}
		//结束Task
		EndTask();
	}
}

void UAbilityTask_WaitSpawnEnemy::OnEnemyClassLoaded()
{
	UClass* LoadedClass=CachedSoftEnemyClassToSpawn.Get();
	UWorld* World=GetWorld();
	
	if (!LoadedClass || !World)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			//无法生成，返回空数组
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}
		EndTask();
		return ;
	}
	//一个临时变量存储所有生成的单位
	TArray<AWarriorEnemyCharacter*> SpawnedEnemies;

	//SpawnActor时为Actor赋予的参数
	FActorSpawnParameters SpawnParameters;
	//Actor生成时的碰撞处理策略
	//尽量调整位置避免碰撞，但如果调整后仍有碰撞，仍然强制生成Actor
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	//一次Task进行Spawn的次数
	for (int32 i =0;i<CachedNumToSpawn;++i)
	{
		FVector RandomLocation;
		//CachedSpawnOrigin为中心生成点，传入BossLocation，RandomLocation是输出参数：point的位置，CachedRandomSpawnRadius是搜索半径
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this,CachedSpawnOrigin,RandomLocation,CachedRandomSpawnRadius);
		//确保角色不会卡在地面中
		RandomLocation+=FVector(0.f,0.f,50.f);

		//在指定位置以指定旋转朝向生成Class类的单位
		AWarriorEnemyCharacter* SpawnedEnemy=World->SpawnActor<AWarriorEnemyCharacter>(LoadedClass,RandomLocation,CachedSpawnRotation);
		
		if (SpawnedEnemy)
		{
			SpawnedEnemies.Add(SpawnedEnemy);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (!SpawnedEnemies.IsEmpty())
		{
			OnSpawnFinished.Broadcast(SpawnedEnemies);
		}
		else
		{
			DidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}

		EndTask();
	}
	
}
