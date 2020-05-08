#pragma once

#include "../MQ2Plugin.h"
#include "Core.h"
// BaseElixir is used as a basis for all class inheritence.
class BaseElixir {
public:
	int StanceMode;

	virtual bool CastGroupHeal(PSPAWNINFO pSpawn) { return false; }
	virtual bool CastPriorityHeal(PSPAWNINFO pSpawn) { return false; }
	virtual bool CastGroupPriorityHeal(PSPAWNINFO pSpawn) { return false; }
	virtual bool CastHeal(PSPAWNINFO pSpawn) { return false; }
	virtual bool CastCombatBuff() { return false; }
	virtual bool CastIdleBuff() { return false; }


	signed int Class() { return classID; }
	
	bool Logic();
protected:
	int classID = 0;
private:
	int highPctHPs = 90;
	int lowPctHPs = 45;

	bool Target();
	bool Spell();
	bool Ability();
	bool Item();
	bool Heal();
	bool SelfHeal();
	bool GroupHeal();
	bool RaidHeal();	
	bool Buff();
	bool IdleBuff();
	bool CombatBuff();
};

