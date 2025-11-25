// Yu

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WarriorAIController.generated.h"

class UAISenseConfig_Damage;
class UAISenseConfig_Hearing;
struct FAIStimulus;
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
/**
 * 
 */
UCLASS()
class WARRIOR_API AWarriorAIController : public AAIController
{
	GENERATED_BODY()
public:
	AWarriorAIController(const FObjectInitializer& ObjectInitializer);
	
	//感知范围内新增对象时进行一次判断
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

protected:
	virtual void BeginPlay() override;
	
	//感知组件
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UAIPerceptionComponent* EnemyPerceptionComponent;
	
	//视觉感知配置
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UAISenseConfig_Sight* AISenseConfig_Sight;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UAISenseConfig_Hearing* AISenseConfig_Hear;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UAISenseConfig_Damage* AISenseConfig_Damage;
	
	UFUNCTION()
	//当感知对象更新时更新
	virtual void OnEnemyPerceptionUpdated( AActor* Actor, FAIStimulus Stimulus);

protected:
	//允许避让
	UPROPERTY(EditDefaultsOnly,Category="Detour Crowd Avoidance Config")
	bool bEnableDetourCrowdAvoidance=true;
	
	//四个级别的避让质量
	UPROPERTY(EditDefaultsOnly,Category="Detour Crowd Avoidance Config",meta=(EditCondition="bEnableDetourCrowdAvoidance",UIMin="1",UIMax="4"))
	int32 DetourCrowdAvoidanceQuality=4;
	
	//碰撞避让检测，在这个范围内触发避让
	UPROPERTY(EditDefaultsOnly,Category="Detour Crowd Avoidance Config",meta=(EditCondition="bEnableDetourCrowdAvoidance"))
	float CollisionQueryRange=700.f;

	UPROPERTY(EditAnywhere,Category="Sight")
	float VisionAngleDegrees=180.f;

	UPROPERTY(EditAnywhere,Category="Sight")
	float SightRadius=2000.f;

	UPROPERTY(EditAnywhere,Category="Sight")
	float LoseSightRadius=2300.f;

	UPROPERTY(EditAnywhere,Category="Sight")
	float SightMaxAge=3.f;

	
	UPROPERTY(EditAnywhere,Category="Hearing")
	float HearRadius=500.f;

	UPROPERTY(EditAnywhere,Category="Hearing")
	float HearMaxAge=3.f;

	UPROPERTY(EditAnywhere,Category="Damage")
	float DamageMaxAge=3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detection")
	bool bDetectEnemies = true;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detection")
	bool bDetectNeutrals = true;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detection")
	bool bDetectFriendlies = false;
	 
};
