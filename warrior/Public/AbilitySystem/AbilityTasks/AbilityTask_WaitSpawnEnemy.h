// Yu

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitSpawnEnemy.generated.h"

class AWarriorEnemyCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitSpawnEnemyDelegate,const TArray<AWarriorEnemyCharacter*>&, SpawnedEnmey);


/**
 * 
 */
UCLASS()
class WARRIOR_API UAbilityTask_WaitSpawnEnemy : public UAbilityTask
{
	GENERATED_BODY()

public:
	//~Begin UGameplayTask Interface

	//Task被激活时执行
	virtual void Activate() override;
	//Task被摧毁时执行
	virtual void OnDestroy(bool bInOwnerFinished) override;
	
	//~End UGameplayTask Interface

	//静态工厂函数，蓝图中创建一个可调用的节点
	UFUNCTION(BlueprintCallable,Category = "Ability|AbilityTasks",meta=(Displayname="Wait GamePlay Event And Spawn Enemies",HidePin="OwningAbility",DefaultToSelf="OwningAbility",BlueprintInternalUseOnly="true",NumToSpawn="1",RandomSpawnRadius="200"))
	static UAbilityTask_WaitSpawnEnemy* WaitSpawnEnemy(
		UGameplayAbility* OwningAbility,
		FGameplayTag EventTag,
		TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn,
		int32 NumToSpawn,
		const FVector& SpawnOrigin,
		float RandomSpawnRadius,
		const FRotator& SpawnRotation);

	/**  UCLASS自动识别BlueprintAssignable，将这两个委托转换为蓝图节点的输出引脚  **/

	//当生成完成时调用
	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemyDelegate OnSpawnFinished;

	//Task执行但是没有生成对应Class时执行
	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemyDelegate DidNotSpawn;

	
	
private:
	FGameplayTag CachedEventTag;
	int32 CachedNumToSpawn;
	float CachedRandomSpawnRadius;
	FVector CachedSpawnOrigin;
	FRotator CachedSpawnRotation;
	TSoftClassPtr<AWarriorEnemyCharacter> CachedSoftEnemyClassToSpawn;
	FDelegateHandle DelegateHandle;
	
	//当对应事件被接收时，触发所有监听委托的回调函数，最后EndTask
	void OnGameplayEventReceived(const FGameplayEventData* InPayLoad);
	
	//当地方单位被加载时
	 void OnEnemyClassLoaded();
};
