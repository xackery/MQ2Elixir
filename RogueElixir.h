#pragma once
#include "../MQ2Plugin.h"
#include "BaseElixir.h"

class RogueElixir : public BaseElixir {
public:
	signed int GetClass() { return Rogue; }

	//bool CastHeal(PSPAWNINFO pSpawn);
	//bool CastPriorityHeal(PSPAWNINFO pSpawn);
	//bool CastGroupHeal(PSPAWNINFO pSpawn);
	//bool CastGroupPriorityHeal(PSPAWNINFO pSpawn);
	//bool CastIdleBuff();
	//bool CastCombatBuff();
};
