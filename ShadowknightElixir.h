#pragma once
#include "../MQ2Plugin.h"
#include "BaseElixir.h"

class ShadowknightElixir : public BaseElixir {
public:
	ShadowknightElixir() { classID = Shadowknight;  }

	bool CastHeal(PSPAWNINFO pSpawn);
	bool CastPriorityHeal(PSPAWNINFO pSpawn);
	//bool CastGroupHeal(PSPAWNINFO pSpawn);
	//bool CastGroupPriorityHeal(PSPAWNINFO pSpawn);
	bool CastIdleBuff();
	bool CastCombatBuff();
};
