#pragma once
#include "../MQ2Plugin.h"
#include "BaseElixir.h"

class DruidElixir : public BaseElixir {
public:
	signed int GetClass() { return Druid; }

	//bool CastHeal(PSPAWNINFO pSpawn);
	//bool CastPriorityHeal(PSPAWNINFO pSpawn);
	//bool CastGroupHeal(PSPAWNINFO pSpawn);
	//bool CastGroupPriorityHeal(PSPAWNINFO pSpawn);
	//bool CastIdleBuff();
	//bool CastCombatBuff();
};
