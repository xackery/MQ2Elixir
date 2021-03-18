#include "../MQ2Plugin.h"
#include "ShamanElixir.h"

bool ShamanElixir::CastGroupHeal(PSPAWNINFO pSpawn) 
{
	PCHARINFO pChar = GetCharInfo();
	if (pChar->pSpawn->GetClass() != Shaman) return false;
	if (!pSpawn) {
		DebugSpewAlways("MQ2Elixir::CastGroupHealShaman pSpawn is nullptr");
		return false;
	}

	if (SpawnPctHPs(pSpawn) <= 55) {
		PCHAR szInterventions[] = {
			"Primordial Intervention", //113 t13
			"Prehistoric Intervention", //108 t13
			"Historian's Intervention", //103 t13
			"Antecessor's Intervention", //98 t13
			"Progenitor's Intervention", //93 t13
			"Ascendant's Intervention", //88 t13
			"Antecedent's Intervention", //83 t13
			"Ancestral Intervention", //78 t13
		};
		for (PCHAR szName : szInterventions) {
			if (!ActionCastSpell(szName)) continue;
			return true;
		}
		PCHAR szName = "Soothsayer's Intervention";
		if (ActionCastAA(szName)) {
			DebugSpewAlways("MQ2Elixir::CastGroupHealShaman using AA %s", szName);
			return true;
		}
	}

	return false;
}

bool ShamanElixir::CastGroupPriorityHeal(PSPAWNINFO pSpawn)
{
	PCHAR szName = "Soothsayer's Intervention";
	if (ActionCastAA(szName)) {
		DebugSpewAlways("MQ2Elixir::CastGroupHealShaman using AA %s", szName);
		return true;
	}

	return false;
}

bool ShamanElixir::CastHeal(PSPAWNINFO pSpawn)
{
	if (!pSpawn) {
		DebugSpewAlways("MQ2Elixir::CastGroupHealShaman pSpawn is nullptr");
		return false;
	}

	PCHAR szReckless[] = {
		"Reckless Renewal", //115
		"Reckless Rejuvenation", //110
		"Reckless Regeneration", //105
		"Reckless Restoration", //100
		"Reckless Remedy", //95
		"Reckless Mending", //90
	};
	for (PCHAR szName : szReckless) {
		if (ActionCastSpell(szName)) {
			WriteChatf("[MQ2Elixir] %s -> %s (%d%%)", szName, pSpawn->Name, SpawnPctHPs(pSpawn));
			DebugSpewAlways("MQ2Elixir::CastGroupHealShaman casting %s", szName);
			return true;
		}
	}

	if (ActionCastItem("Zrelik's Mending")) return true;
	return false;
}

bool ShamanElixir::CastCombatBuff()
{
	bool isBuffed;
	PCHAR szName;
	PCHARINFO pChar = GetCharInfo();
	if (pChar->pSpawn->GetClass() != Shaman) return false;
	PCHAR szDissidentRoars[] = {
		"Dissident Roar 1",
		"Dissident Roar 2",
		"Dissident Roar 3",
		"Dissident Roar 4",
		"Dissident Roar 5",
		"Dissident Roar 6",
		"Roar of the Lion 1",
		"Roar of the Lion 2",
		"Roar of the Lion 3",
		"Roar of the Lion 4",
		"Roar of the Lion 5",
		"Roar of the Lion 6",
	};
	isBuffed = false;
	for (PCHAR szName : szDissidentRoars) {
		if (!HasBuff(pChar->pSpawn, szName)) continue;
		isBuffed = true;
		
	}
	if (!isBuffed) {
		szName = "Dissident Roar";
		if (ActionCastSpell(szName)) {
			WriteChatf("[MQ2Elixir] %s", szName);
			DebugSpewAlways("MQ2Elixir::CastCombatBuff %s", szName);
			return true;
		}
	}
	return false;
}
