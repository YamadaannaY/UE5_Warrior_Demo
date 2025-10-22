// Yu


#pragma once
#include "NativeGameplayTags.h"

//纯c++命名空间
namespace  WarriorGamePlayTags
{
	/**      Input Tags：描述输入来源，告诉系统玩家进行了输入
	 *       1.在DA_InputConfig中与Input action绑定
	 *       2.Axe相关InputTag在BP_Axe的WeaponData中与GA绑定
	 *       3.InputTag_SwitchTarget单独有新的MIC接受，优先级最高，在锁定目标时实现
	 *		 两个无后缀的用于所有此前缀下标签对应GA的统一逻辑实现。
	 *       **/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_UnequipAxe);	
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_equipAxe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_LightAttack_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_HeavyAttack_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Roll);
	
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MustBeHeld);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MustBeHeld_Block);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MustBeHeld_Sprint);
	
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Toggleable);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Toggleable_TargetLock);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Toggleable_Rage);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_SwitchTarget);
	/** ----------------------------------------------------------------------------- **/

	/**		Ability Tags 调用时机：于GA中的AssetTags/BlockAbilities选择调用，用于标记/禁用玩家的GA**/
	//		Player Ability Tags：描述具体GA，告诉系统这是玩家身上的GA **/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Equip_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Unequip_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Light_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Heavy_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_HitPause);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Roll);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Block);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Sprint);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_TargetLock);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Rage);
	//		Enemy Ability Tags：描述具体GA，告诉系统这是敌人身上的GA **/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Melee);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Ranged);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_SummonEnemy);
	//		Shared Ability Tags：共有GA
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Ability_HitReact)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Ability_Death)
	/** ----------------------------------------------------------------------------- **/
	
	//		Weapon Tag:包装进CombatComponent中的WeaponMap,作为武器的Tag存在
	//		Player Weapon Tags   **/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Weapon_Axe);
	//		Enemy Weapon Tag	**/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Weapon);
	/** ----------------------------------------------------------------------------- **/
	
	/**		Event Tag :与AN绑定/C++蓝图中使用UAbilitySystemBlueprintLibrary::SendGameplayEventToActor
	 *		用于作为蓝图WaitGameplayEvent的触发参数,被触发后调用EventReceived之后的逻辑	**/
	//		Player Event Tag
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Equip_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Unequip_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_HitPause);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_SuccessfulBlock);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_SwitchTarget_Left);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_SwitchTarget_Right);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_ActivateRage);
	//		Enemy Event Tag
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Event_SummonEnemy);
	//		Shared Event Tag：适用于敌我的Event Tag
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_MeleeHit)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_HitReact)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_SpawnProjectile)
	/** ----------------------------------------------------------------------------- **/

	/**		Status tag：用于赋予角色状态Tag，使得角色在判断中获得额外的逻辑作用**/
	//Player Status tag
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_JumpToFinisher);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rolling);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_Dead);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Blocking);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_TargetLock);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rage_Activating);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rage_Active);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rage_Full);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rage_None);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Invincible);
	//Enemy Status tag
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Status_UnderAttack);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Status_Strafing);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Status_UnBlockable);
	//Shared Status tag
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Front)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Left)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Right)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Back)
	
	/** ----------------------------------------------------------------------------- **/
	
	/**		专门用于MakeHeroDamageEffectSpecHandle中作为函数引脚，在基础伤害逻辑、轻击GA中和重击GA中分别使用这三个Tag
	 *		被GA_DealDamage调用，计算伤害加成**/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_SetByCaller_BaseDamage)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_SetByCaller_AttackType_Light);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_SetByCaller_AttackType_Heavy);

	
}
