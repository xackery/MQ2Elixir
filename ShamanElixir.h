#pragma once

#include "BaseElixir.h"

class ShamanElixir : public BaseElixir {
public:
	signed int GetClass() { return Shaman; }

	bool CastHeal(PSPAWNINFO pSpawn);
	//bool CastPriorityHeal(PSPAWNINFO pSpawn);
	bool CastGroupHeal(PSPAWNINFO pSpawn);
	bool CastGroupPriorityHeal(PSPAWNINFO pSpawn);
	//bool CastIdleBuff();
	bool CastCombatBuff();
};

