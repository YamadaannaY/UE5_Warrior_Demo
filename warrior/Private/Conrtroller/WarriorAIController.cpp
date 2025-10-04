// Yu


#include "Conrtroller/WarriorAIController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "WarriorDebugHelper.h"

//~ :Super是初始化列表一种方式：调用父类的构造函数，在父类构造函数时就默认把PathFollowingComponent（路径跟随组件）改成CrowdFollowingComponent
AWarriorAIController::AWarriorAIController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	AISenseConfig_Sight=CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies=true; // 能看见敌人
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies=false; // 不检测友军
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals=false;  // 不检测中立单位
	AISenseConfig_Sight->SightRadius=5000.f; // 视野半径
	AISenseConfig_Sight->LoseSightRadius=0.f; // 丢失目标距离（通常应大于 SightRadius，这里设 0 意味着一旦出视野就立刻丢失）
	AISenseConfig_Sight->PeripheralVisionAngleDegrees=90.f; // 视野角度（0° = 只能正前方一条线，通常会设置为 90-120°）

	SetPerceptionComponent(*EnemyPerceptionComponent);
	EnemyPerceptionComponent=CreateDefaultSubobject<UAIPerceptionComponent>("EnemyAIPerceptionComponent");
	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight); //感知配置
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass()); //主导感知类型，多感官下优先使用这个结果
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this,&ThisClass::OnEnemyPerceptionUpdated); //绑定感知更新事件，当AI感知到目标（进入/离开视野）时，就会回调OnEn...函数，其中更新TargetActor

	AAIController::SetGenericTeamId(FGenericTeamId(1));
}

//设置ETeamAttitude
ETeamAttitude::Type AWarriorAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	//设置
	const APawn* PawnToCheck=Cast<const APawn>(&Other);
	const IGenericTeamAgentInterface* OtherTeamAgent=Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetController());
	//如果TeamId不同，则判断为Hostile。
	if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId()< GetGenericTeamId())
	{
		return ETeamAttitude::Hostile;
	}
	return ETeamAttitude::Friendly;
}

void AWarriorAIController::BeginPlay()
{
	Super::BeginPlay();
	//虽然构造函数中对默认PathFollowingComponent()转换成了CrowdFollowingComponent，但是这个Get函数返回的是PathFollowingComponent()，所以需要Cast
	if (UCrowdFollowingComponent* CrowdComp=Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		CrowdComp->SetCrowdSimulationState(bEnableDetourCrowdAvoidance ? ECrowdSimulationState::Enabled : ECrowdSimulationState::Disabled);
		switch (DetourCrowdAvoidanceQuality)
		{
			//设置CrowdAvoidance的质量，越高越精确，但是Performance消耗也越大
			case 1:CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low); break;
			case 2:CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium); break;
			case 3:CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good); break;
			case 4:CrowdComp->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High); break;
			default:break;
		}
		CrowdComp->SetAvoidanceGroup(1); //设置自己属于哪一组避让
		CrowdComp->SetGroupsToAvoid(1); //设置要避让的组
		CrowdComp->SetCrowdCollisionQueryRange(CollisionQueryRange); //检测附近其他代理的范围
	}
	
}

//当感知更新时，设置FName变量名对应变量的具体参数
void AWarriorAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (UBlackboardComponent*BlackboardComponent=GetBlackboardComponent())
	{
		if (!BlackboardComponent->GetValueAsObject(FName("TargetActor")))
		{
			if (Stimulus.WasSuccessfullySensed() && Actor)
			{	
					//SetValueAsObject设置Object及其子类参数所用
				BlackboardComponent->SetValueAsObject(FName("TargetActor"),Actor);
			}
		}
	}
}
