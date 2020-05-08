#pragma once
#include "../MQ2Plugin.h"
#include "BaseElixir.h"

class PaladinElixir : public BaseElixir {
public:
	signed int Class() { return Paladin; }

	//bool CastHeal(PSPAWNINFO pSpawn);
	//bool CastPriorityHeal(PSPAWNINFO pSpawn);
	//bool CastGroupHeal(PSPAWNINFO pSpawn);
	//bool CastGroupPriorityHeal(PSPAWNINFO pSpawn);
	//bool CastIdleBuff();
	//bool CastCombatBuff();
};
