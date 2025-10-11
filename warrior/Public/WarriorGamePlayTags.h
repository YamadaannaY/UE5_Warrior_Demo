// Yu


#pragma once
#include "NativeGameplayTags.h"

//纯c++命名空间
namespace  WarriorGamePlayTags
{
	/**      Input Tags：描述输入来源，告诉系统玩家进行了输入
	 *       调用时机：1.在InputConfig中与input action绑定 2.Axe相关InputTag在BP_Axe的weapon abilities中与GA绑定，      **/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_UnequipAxe);	
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_equipAxe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_LightAttack_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_HeavyAttack_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Roll);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MustBeHeld);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_MustBeHeld_Block);

	/**  Player Ability Tags：描述具体能力，告诉系统这是玩家身上的能力 调用时机：与GA中的AssetTags/BlockAbilities 绑定  **/ 
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Equip_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Unequip_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Light_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Heavy_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_HitPause);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Roll);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Block);
	
	/**Player Tags   **/
	//Weapon:包装进CombatComponent中的WeaponMap
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Weapon_Axe);

	/** Enemy Tags   **/
	//Weapon:包装进CombatComponent中的WeaponMap
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Weapon);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Melee);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Ability_Ranged);

	/**   Event Tag :与anim notify 绑定，用于作为蓝图节点wait Gameplay event的节点**/
	/**   调用时机：当动画运行到notify被触发的那一帧时执行event received中的操作             **/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Equip_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_Unequip_Axe);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_HitPause);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Event_SuccessfulBlock);

	/** status tag：用于赋予角色状态Tag 
	 *	调用时机：在角色普攻第三段赋予角色标签，此时在重击GA中检索此Tag，如果有，且触发重击则将重击计数直接赋值到最后一段	**/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_JumpToFinisher);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Rolling);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_Dead);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Status_UnderAttack);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Enemy_Status_Strafing);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Status_Blocking);

	
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_SetByCaller_AttackType_Light);
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_SetByCaller_AttackType_Heavy);

	/**Shared Tag**/
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_MeleeHit)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Event_HitReact)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Ability_HitReact)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Ability_Death)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Front)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Left)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Right)
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_Status_HitReact_Back)
	
	WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shared_SetByCaller_BaseDamage)
	
}
