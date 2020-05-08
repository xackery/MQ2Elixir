#pragma once
#include "../MQ2Plugin.h"
#include "BaseElixir.h"

class NecromancerElixir : public BaseElixir {
public:
	signed int Class() { return Necromancer; }

	//bool CastHeal(PSPAWNINFO pSpawn);
	//bool CastPriorityHeal(PSPAWNINFO pSpawn);
	//bool CastGroupHeal(PSPAWNINFO pSpawn);
	//bool CastGroupPriorityHeal(PSPAWNINFO pSpawn);
	//bool CastIdleBuff();
	//bool CastCombatBuff();
};
