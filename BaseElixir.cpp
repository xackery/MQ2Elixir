#include "../MQ2Plugin.h"
#include "Core.h"
#include "BaseElixir.h"

bool BaseElixir::Logic()
{
	if (Target()) return true;
	if (Heal()) return true;
	if (Buff()) return true;
	return false;
}

bool BaseElixir::Heal()
{
	//DebugSpewAlways("MQ2Elixir::Heal");
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) return false;
	bool isBard = (pChar->pSpawn->GetClass() == Bard);

	if (pChar->pSpawn->Type != SPAWN_PLAYER) return false;

	if (AreObstructionWindowsVisible()) return false;
	if (pChar->pSpawn->StandState == STANDSTATE_FEIGN) return false;
	if (!IsCastingReady()) return false;
	if (!isBard && IsMoving(pChar->pSpawn)) return false;
	if (pChar->pSpawn->HideMode) return false;
	if (SelfHeal()) return true;
	if (GroupHeal()) return true;
	if (RaidHeal()) return true;
	return false;
}


bool BaseElixir::SelfHeal() 
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) return false;

	//DebugSpewAlways("MQ2Elixir::SelfHealCheck");

	//Healer support classes heal self along with group, 
	// so skip to group heal checks if in group
	if ((pChar->pSpawn->GetClass() == Cleric ||
		pChar->pSpawn->GetClass() == Shaman ||
		pChar->pSpawn->GetClass() == Druid ||
		pChar->pSpawn->GetClass() == Paladin) &&
		pChar->pGroupInfo) {
		return false;
	}
	//TODO: check if in raid

	if (SpawnPctHPs(pChar->pSpawn) >= highPctHPs) return false;

	bool isSelfTargetHealer = false;
	if (pChar->pSpawn->GetClass() != Necromancer &&
		pChar->pSpawn->GetClass() != Shadowknight) {
		lowPctHPs = 75;
		isSelfTargetHealer = true;
	}

	if (isSelfTargetHealer && !ActionSpawnTarget(pChar->pSpawn)) return false;

	int healTargetPctHPs = SpawnPctHPs(pChar->pSpawn);
	int hpDifference = 0;

	//DebugSpewAlways("MQ2Elixir::SelfHealCheck hp low, determine spell");
	if ((healTargetPctHPs <= lowPctHPs || hpDifference > 40) && CastPriorityHeal(pChar->pSpawn))  return true;
	if (CastHeal(pChar->pSpawn)) return true;
	return false;
}

bool BaseElixir::GroupHeal()
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) return false;
	if (!pChar->pGroupInfo) return false;
	return false;


	int healCount = 0;
	int healTargetPctHPs = 100;
	int hpDifference = 0;
	PSPAWNINFO healTargetSpawn;

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
		if (pSpawn->Type == SPAWN_CORPSE) continue;
		spawnPctHPs = SpawnPctHPs(pSpawn);
		WriteChatf("[MQ2Elixir] %s %d", pSpawn->Name, spawnPctHPs);

		if (Distance3DToSpawn(pChar, pSpawn) > 200) continue;
		if (spawnPctHPs <= highPctHPs) healCount++;

		if (healTargetPctHPs < spawnPctHPs) continue;

		healTargetPctHPs = spawnPctHPs;
		healTargetSpawn = pSpawn;
	}

	XTARGETSLOT xts;
	ExtendedTargetList* xtm = pChar->pXTargetMgr;
	if (!xtm) return false;

	for (int i = 0; i < xtm->XTargetSlots.Count; i++) {
		xts = xtm->XTargetSlots[i];
		if (!xts.SpawnID) continue;
		if (xts.xTargetType != XTARGET_SPECIFIC_NPC &&
			xts.xTargetType == XTARGET_SPECIFIC_PC) continue;

		pSpawn = (PSPAWNINFO)GetSpawnByID(xts.SpawnID);
		if (!pSpawn) continue;
		if (pSpawn->Type == SPAWN_CORPSE) continue;
		spawnPctHPs = SpawnPctHPs(pSpawn);
		if (Distance3DToSpawn(pChar, pSpawn) > 200) continue;
		if (spawnPctHPs <= highPctHPs) healCount++;
		if (healTargetPctHPs < spawnPctHPs) continue;

		healTargetPctHPs = spawnPctHPs;
		healTargetSpawn = pSpawn;
	}

	//TODO: hp difference calculator

	if (!healTargetSpawn) return false;

	bool isSelfTargetHealer = false;
	if (pChar->pSpawn->GetClass() != Necromancer &&
		pChar->pSpawn->GetClass() != Shadowknight) {
		isSelfTargetHealer = true;
	}

	WriteChatf("[MQ2Elixir] heal needed for %", healTargetSpawn->Name);

	if (isSelfTargetHealer && healTargetSpawn->SpawnID == pChar->pSpawn->SpawnID && !ActionSpawnTarget(healTargetSpawn)) {
		DebugSpewAlways("MQ2Elixir::GroupHealCheck could not target %s", healTargetSpawn->Name);
		return false;
	}
	if ((healTargetPctHPs <= lowPctHPs || hpDifference > 40) && healCount > 1 && CastGroupPriorityHeal(healTargetSpawn))  return true;
	if ((healTargetPctHPs <= lowPctHPs || hpDifference > 40) && CastPriorityHeal(healTargetSpawn))  return true;
	if (healCount > 1 && CastGroupHeal(healTargetSpawn)) return true;
	if (CastHeal(healTargetSpawn)) return true;


	return false;
}

bool BaseElixir::RaidHeal()
{
	return false;
}


bool BaseElixir::Target()
{
	return false;
}

bool BaseElixir::Buff()
{
	if (IdleBuff()) return true;
	if (CombatBuff()) return true;
	return false;
}

bool BaseElixir::IdleBuff()
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) return false;
	bool isBard = (pChar->pSpawn->GetClass() == Bard);

	if (AreObstructionWindowsVisible()) return false;
	if (pChar->pSpawn->StandState == STANDSTATE_FEIGN) return false;
	if (XTargetNearbyHaterCount()) return false;
	if (!isBard && IsMoving(pChar->pSpawn)) return false;
	if (IsNavActive()) return false;
	if (IsInSafeZone()) return false;
	if (pChar->pSpawn->HideMode) return false;
	if (CombatState() != COMBATSTATE_ACTIVE && CombatState() != COMBATSTATE_COOLDOWN && CombatState() != COMBATSTATE_RESTING) return false;
	if (CastIdleBuff()) return true;
	return false;
}

bool BaseElixir::CombatBuff()
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) return false;
	bool isBard = (pChar->pSpawn->GetClass() == Bard);

	if (AreObstructionWindowsVisible()) return false;
	if (pChar->pSpawn->StandState == STANDSTATE_FEIGN) return false;
	if (pChar->pSpawn->StandState == STANDSTATE_SIT) return false;
	if (!XTargetNearbyHaterCount()) return false;
	//if (!pTarget) return false;
	//if (GetDistance(pChar->pSpawn, (PSPAWNINFO)pTarget) > 30) return false;
	if (!isBard && IsMoving(pChar->pSpawn)) return false;
	//if (pTarget->Data.Type != SPAWN_NPC) return false;
	if (IsNavActive()) return false;
	if (!LineOfSight(pChar->pSpawn, (PSPAWNINFO)pTarget)) return false;
	if (IsInSafeZone()) return false;
	if (pChar->pSpawn->HideMode) return false;
	if (CombatState() != COMBATSTATE_COMBAT) return false;
	if (CastCombatBuff()) return true;
	return false;
}

