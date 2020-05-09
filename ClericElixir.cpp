#include "../MQ2Plugin.h"
#include "ClericElixir.h"

bool ClericElixir::CastIdleBuff()
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) return false;
	if (pChar->pSpawn->GetClass() != Cleric) return false;
	PCHAR szName;

	// HP Self Buffs
	PCHAR szNames[] = { "Armor of Sincerity", //115
		"Armor of the Merciful", //110
		"Armor of the Ardent", //105
		"Armor of the Reverent", //100
		"Armor of the Zealous", //95
		"Armor of the Earnest", //90
		"Armor of the Devout", //85
		"Armor of the Solemn", //80
		"Armor of the Sacred", //75
		"Armor of the Pious", //70
		"Armor of the Zealot", //65
		"Ancient: High Priest's Bulwark",
		"Blessed Armor of the Risen", //58
		"Armor of the Faithful", //49
		"Armor of Protection", //34
	};
	for (PCHAR szName : szNames) {
		if (!HasBuff(pChar->pSpawn, szName)) continue;
		//TODO: is there an AA version of this?

		if (ActionCastSpell(szName)) {
			DebugSpewAlways("MQ2Elixir::ClericIdleBuffs casting %s", szName);
			return true;
		}
		if (!IsSpellMemorized(szName) && ActionMemorizeSpell(12, szName)) {
			DebugSpewAlways("MQ2Elixir::ClericIdleBuffs memorizing %s", szName);
			return true;
		}
		if (!IsSpellReady(szName) && SpellCooldown(szName)) {
			DebugSpewAlways("MQ2Elixir::ClericIdleBuffs waiting on cooldown of %s", szName);
			return false;
		}
	}

	GROUPMEMBER* pG;
	PSPAWNINFO pSpawn;
	int spawnPctHPs = 0;
	for (int i = 1; i < 6; i++) {
		pG = GetCharInfo()->pGroupInfo->pMember[i];
		if (!pG) continue;
		if (pG->Offline) continue;
		//if (${Group.Member[${i}].Mercenary}) /continue
		//if (${ Group.Member[${i}].OtherZone }) / continue		
		pSpawn = pG->pSpawn;
		if (!pSpawn) continue;
		if (pSpawn->SpawnID == pChar->pSpawn->SpawnID) continue;
		if (pSpawn->Type == SPAWN_CORPSE) continue;

		if (Distance3DToSpawn(pChar, pSpawn) > 200) continue;

		szName = "Unified Hand of Righteousness";
		if (!HasBuff(pChar->pSpawn, "Righteousness") && ActionCastSpell(szName)) {
			DebugSpewAlways("MQ2Elixir::ClericIdleBuffs casting %s", szName);
			return true;
		}

		szName = "Hand of Will";
		if (!HasBuff(pChar->pSpawn, szName) && ActionCastSpell(szName)) {
			DebugSpewAlways("MQ2Elixir::ClericIdleBuffs casting %s", szName);
			return true;
		}

		szName = "Rallied Greater Protection of Vie";
		if (!HasBuff(pChar->pSpawn, szName) && ActionCastSpell(szName)) {
			DebugSpewAlways("MQ2Elixir::ClericIdleBuffs casting %s", szName);
			return true;
		}
	}

	return false;
}

bool ClericElixir::CastGroupPriorityHeal(PSPAWNINFO pSpawn)
{
	if (ActionCastSpell("Dissident Blessing")) return true;
	if (ActionCastAA("Divine Arbitration")) return true;
	if (ActionCastItem("Harmony of the Soul")) return true;
	if (ActionCastItem("Aegis of Superior Divinity")) return true;
	return false;
}


bool ClericElixir::CastGroupHeal(PSPAWNINFO pSpawn)
{
	if (ActionCastSpell("Word of Greater Replenishment")) return true;
	if (ActionCastAA("Celestial Regeneration")) return true;
	if (ActionCastItem("Harmony of the Soul")) return true;
	if (ActionCastItem("Aegis of Superior Divinity")) return true;
	if (ActionCastSpell("Dissident Blessing")) return true;
	if (ActionCastAA("Divine Arbitration")) return true;
	return false;
}


bool ClericElixir::CastPriorityHeal(PSPAWNINFO pSpawn)
{
	if (ActionCastSpell("Dissident Blessing")) return true;
	if (ActionCastAA("Beacon of Life")) return true;
	if (ActionCastAA("Focused Celestial Regeneration")) return true;

	if (ActionCastItem("Harmony of the Soul")) return true;
	if (ActionCastItem("Aegis of Superior Divinity")) return true;

	if (ActionCastAA("Focused Celestial Regeneration")) return true;
	return false;
}

bool ClericElixir::CastHeal(PSPAWNINFO pSpawn)
{
	if (ActionCastSpell("Sincere Remedy")) return true;
	if (ActionCastSpell("Merciful Remedy")) return true;
	if (ActionCastSpell("Spiritual Remedy")) return true;
	if (ActionCastSpell("Graceful Remedy")) return true;

	if (ActionCastAA("Focused Celestial Regeneration")) return true;
	//if (ActionCastSpell("Sincere Intervention")) return true;
	return false;
}

