#include "../MQ2Plugin.h"
#include "ShadowknightElixir.h"

bool ShadowknightElixir::CastHeal(PSPAWNINFO pSpawn)
{
	if (!pTarget) {
		DebugSpewAlways("MQ2Elixir::CastHealShadowknight pSpawn is nullptr");
		//TODO: in cases pTarget is not set, we need to discern a lifetap target
		return false;
	}

	PCHAR szNames[] = {
		"Touch of Drendar", //115 7869
		"Cadcane's Lifedraw", //113 5053
		"Touch of T`Vem", //110 6488
		"Touch of Lutzen", //105 5350
		"Vizat's Lifedraw", //103 3435
		"Touch of Falsin", //100 4043
		"Grelleth's Lifedraw", //98 2769
		"Unholy Guardian Lifetap", //97 3126
		"Insolent Agitation", //97 2438
		"Touch of Urash", //95 3334
		"Sholothian Lifedraw", //93 2283
		"Touch of Dyalgem", //90 2749
		"Gorgon Lifedraw", //88 1882
		"Touch of Tharoff", //85
		"Touch of Kildrukaun", //80
		"Touch of Severan", //75
		"Drink of Decomposition", //73
		"Touch of the Devourer", //70
		"Touch of Inruku", //67
		"Touch of Innoruuk", //65
		"Touch of Volatis", //62
		"Drain Soul", //60
		"Drain Spirit", //57
		"Spirit Tap", //55
		"Siphon Life", //51
		"Life Leech", //47
		"Lifedraw", //29
		"Lifespike", //15
		"Lifetap" //8
	};
	for (PCHAR szName : szNames) {
		if (ActionCastSpell(szName)) {
			DebugSpewAlways("MQ2Elixir::SHDCastHeal casting %s", szName);
			return true;
		}
	}

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

	if (ActionCastAA("Leech Touch")) return true;

	PCHAR szNames[] = {
		"Dissident Fang", //106 15000
		"Dire Indictment", //115 14371
		"Dire Testimony", //110 11850
		"Dire Declaration", //105 9772
		"Dire Insinuation", //100 7879
		"Dire Allegation", //95 6497
		"Dire Accusation", //90 5357
		"Dire Implication", //85 4427
	};
	for (PCHAR szName : szNames) {
		if (ActionCastSpell(szName)) {
			DebugSpewAlways("MQ2Elixir::SHDCastHeal casting %s", szName);
			return true;
		}
	}

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
		if (!IsSpellMemorized(szName) && ActionMemorizeSpell(12, szName)) {
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
		if (!IsSpellMemorized(szName) && ActionMemorizeSpell(12, szName)) {
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
	PSPAWNINFO pTargetSpawn = nullptr;
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


	PCHAR szSpears[] = {
		"Spear of Cadcane", //114 16441
		"Spear of Tylix", //109 13556
		"Spear of Vizat", //104 11177
		"Spear of Grelleth", //99 9655
		"Spear of Sholoth", //94 7961
		"Gorgon Spear", //89 6419
		"Malarian Spear", //84 2113
		"Rotmarrow Spear", //79 1377
		"Rotroot Spear", //74 1204
		"Spear of Muram", //69 963
		"Miasmic Spear", //65 770
		"Spear of Decay", //64 700
		"Spear of Plague", //54 444
		"Spear of Pain", //48 325
		"Spear of Disease", //34 251
		"Spike of Disease", //1 15
	};
	for (PCHAR szName : szSpears) {
		if (ActionCastSpell(szName)) {
			DebugSpewAlways("MQ2Elixir::ShadowknightCombatBuffs casting %s", szName);
			return true;
		}
	}


	DebugSpewAlways("EQElixir::ShadowknightCombatBuffs nothing to do");
	return false;
}
