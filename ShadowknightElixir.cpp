#include "../MQ2Plugin.h"
#include "ShadowknightElixir.h"

bool ShadowknightElixir::CastHeal(PSPAWNINFO pSpawn)
{
	if (!pTarget) {
		DebugSpewAlways("MQ2Elixir::CastHealShadowknight pSpawn is NULL");
		//TODO: in cases pTarget is not set, we need to discern a lifetap target
		return false;
	}

	//t0 taps
	if (ActionCastSpell("Touch of Drendar")) return true;
	if (ActionCastSpell("Touch of Lutzen")) return true;
	if (ActionCastSpell("Touch of Zlandicar")) return true;
	DebugSpewAlways("MQ2Elixir::CastHealShadowknight no spells available");
	return false;
}

bool ShadowknightElixir::CastPriorityHeal(PSPAWNINFO pSpawn)
{
	PCHARINFO pChar = GetCharInfo();
	if (pChar->pSpawn->GetClass() != Shadowknight) return false;

	if (!pTarget) {
		//TODO: in cases pTarget is not set, we need to discern a lifetap target
		return false;
	}

	if (ActionCastSpell("Dire Indictment")) return true;
	if (ActionCastSpell("Dissident Fang")) return true;
	if (ActionCastAA("Leech Touch")) return true;
	return false;
}

bool ShadowknightElixir::CastIdleBuff()
{
	PCHARINFO pChar = GetCharInfo();
	if (pChar->pSpawn->GetClass() != Shadowknight) return false;


	PCHAR szName = "Stormwall Stance";
	// Stances need their own reserved gem due to long cooldown
	if (!HasBuff(pChar->pSpawn, szName)) {
		if (ActionCastSpell(szName)) {
			DebugSpewAlways("MQ2Elixir::ShadowknightIdleBuffs casting %s", szName);
			return true;
		}
	}

	szName = "Concordant Disruption";
	if (!HasBuff(pChar->pSpawn, szName)) {
		if (ActionCastSpell(szName)) {
			DebugSpewAlways("MQ2Elixir::ShadowknightIdleBuffs casting %s", szName);
			return true;
		}
		if (!IsSpellMemorized(szName) && ActionMemorizeSpell(szName, 12)) {
			DebugSpewAlways("MQ2Elixir::ShadowknightIdleBuffs memorizing %s", szName);
			return true;
		}
		if (!IsSpellReady(szName) && SpellCooldown(szName)) {
			DebugSpewAlways("MQ2Elixir::ShadowknightIdleBuffs waiting on cooldown of %s", szName);
			return false;
		}
	}

	szName = "Cadcane's Skin";
	if (!HasBuff(pChar->pSpawn, szName)) {
		if (ActionCastSpell(szName)) {
			//DebugSpewAlways("MQ2Elixir::ShadowknightIdleBuffs casting %s", szName);
			return true;
		}
		if (!IsSpellMemorized(szName) && ActionMemorizeSpell(szName, 12)) {
			//DebugSpewAlways("MQ2Elixir::ShadowknightIdleBuffs memorizing %s", szName);
			return true;
		}
		if (!IsSpellReady(szName) && SpellCooldown(szName)) {
			//DebugSpewAlways("MQ2Elixir::ShadowknightIdleBuffs waiting on cooldown of %s", szName);
			return false;
		}
	}

	return false;
}

bool ShadowknightElixir::CastCombatBuff()
{
	PCHARINFO pChar = GetCharInfo();
	if (pChar->pSpawn->GetClass() != Shadowknight) return false;
	bool isMainTank = false;
	if (pChar->pGroupInfo && pChar->pGroupInfo->pMember[0]->MainTank) isMainTank = true;
	PSPAWNINFO pTargetSpawn = NULL;
	if (pTarget) pTargetSpawn = (PSPAWNINFO)GetSpawnByID(pTarget->Data.SpawnID);

	PCHAR szNames[] = { "Thwart", //113 t15
		"Spurn", //108 t15
		"Repel", //103 t15
		"Reprove", //98 t15
		"Renounce", //93 t15
		"Defy", //88 t15
		"Withstand", //83 t15		
	};
	for (PCHAR szName : szNames) {
		if (!HasBuff(pChar->pSpawn, szName) && ActionCastCombatAbility(szName)) return true;
	}

	PCHAR szBlades[] = { "Rending Blade", //112 t10
		"Wounding Blade", //107 t10
		"Lacerating Blade", //102 t10
		"Gashing Blade", //97 t10
		"Gouging Blade", //92 t10
	};
	for (PCHAR szName : szBlades) {
		if (ActionCastCombatAbility(szName)) return true;
	}



	//Target based combat buffs/debuffs
	if (!isMainTank) return false;
	PCHAR szProtest[] = { "Challenge for Power",
		"Trial for Power",
		"Charge for Power",
		"Confrontation for Power",
		"Provocation for Power",
		"Demand for Power",
		"Impose for Power",
		"Refute for Power",
		"Protest for Power",
	};
	for (PCHAR szName : szProtest) {
		if (!pTargetSpawn) continue;
		if (HasBuff(pTargetSpawn, szName)) continue;
		if (!ActionCastSpell(szName)) continue;
		return true;
	}
	DebugSpewAlways("EQElixir::ShadowknightCombatBuffs nothing to do");
	return false;
}
