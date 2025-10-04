// Yu


#include "Conrtroller/WarriorHeroController.h"

AWarriorHeroController::AWarriorHeroController()
{
	HeroTeamId=FGenericTeamId(0);
}

FGenericTeamId AWarriorHeroController::GetGenericTeamId() const
{
	return HeroTeamId;
}
