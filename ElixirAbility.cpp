#include <mq/Plugin.h>
#include "Core.h"
#include "Elixir.h"

using namespace std;


bool isBashAllowed()
{
	if (!GetPcProfile()) return false;
	if (GetPcProfile()->pInventoryArray->Inventory.Secondary && GetItemFromContents(GetPcProfile()->pInventoryArray->Inventory.Secondary)->ItemType == 8) return true;
#if !defined(ROF2EMU) && !defined(UFEMU)
	if (GetPcProfile()->Class == EQData::Warrior && IsAAPurchased("Two-Handed Bash")) return true;
	if (GetPcProfile()->Class == EQData::Paladin || GetPcProfile()->Class == EQData::Shadowknight && IsAAPurchased("Improved Bash")) return true;
#else
	if (IsAAPurchased("2 Hand Bash")) return true;
#endif
	return false;
}

// Ability returns a string with a reason if provided ability cannot be used
std::string Elixir::Ability(int abilityIndex)
{
	CHAR szTemp[MAX_STRING] = { 0 };
	if (!pLocalPC) {
		return "pLocalPC not loaded";
	}

	DWORD nToken = pSkillMgr->GetNameToken(abilityIndex);
	if (!HasSkill(abilityIndex)) {
		return "don't have ability";
	}

	const char* abilityName = pStringTable->getString(nToken, 0);
	if (!abilityName) {
		return "can't find ability";
	}
	if (!pSkillMgr->IsActivatedSkill(abilityIndex)) {
		return "not activated skill (ignored)";
	}

	if (!pSkillMgr->IsAvailable(abilityIndex)) {
		return "on cooldown";
	}

	if (stricmp(abilityName, "Hide") == 0) {
		return "(ignored)";
	}

	// Do logical ability checks


	if (!pTarget) {
		return "no target";
	}

	if (Distance3DToSpawn(pLocalPC->pSpawn, (PSPAWNINFO)pTarget) > 16) {
		return "too far away";
	}

	if (pTarget && !pCharSpawn->CanSee(*pTarget)) {
		return "target not line of sight";
	}

	
	if (pTarget->Type != SPAWN_NPC) {
		return "non npc targetted";
	}


	if (SpawnPctHPs((PSPAWNINFO)pTarget) > NukeAIHPMax) {
		sprintf_s(szTemp, "target > %d%%", NukeAIHPMax);
		return szTemp;
	}

	if (stricmp(abilityName, "Bash") == 0) {
		if (!isBashAllowed()) {
			return "cannot bash";
		}

		return "";
	}

	if (stricmp(abilityName, "Kick") == 0) {
		return "";
	}

	if (stricmp(abilityName, "Disarm") == 0) {
		if (Distance3DToSpawn(pLocalPC->pSpawn, (PSPAWNINFO)pTarget) > 14) {
			return "too far away";
		}
		return "";
	}

	if (stricmp(abilityName, "Bind Wound") == 0) {
		//if (SpawnPctHPs(GetCharInfo()->pSpawn) < 50) {
			//return "not < 50% hp";
		//}
		//return "";
		return "not supported";
	}

	if (stricmp(abilityName, "Taunt") == 0) {
		if (GetCharInfo()->Group == nullptr) {
			return "not in a group";
		}

		if (!pAggroInfo) {
			return "pAggroInfo empty";
		}

		bool isMainTank = false;
		CGroupMember* pG;
		PSPAWNINFO pSpawn;
		for (int i = 0; i < 6; i++) {
			pG = GetCharInfo()->Group->GetGroupMember(i);
			if (!pG) continue;
			if (pG->Offline) continue;
			//if (${Group.Member[${i}].Mercenary}) /continue
			//if (${ Group.Member[${i}].OtherZone }) / continue
			pSpawn = pG->pSpawn;
			if (!pSpawn) continue;
			if (pSpawn->SpawnID != pLocalPC->pSpawn->SpawnID) continue;
			if (pSpawn->Type == SPAWN_CORPSE) continue;
			if (pG->MainTank) {
				isMainTank = true;
				break;
			}
		}

		if (!isMainTank) {
			return "not main tank";
		}

		if (pAggroInfo && pAggroInfo->aggroData[AD_Player].AggroPct >= 100) {
			return "already have high hate";
		}

		if (pAggroInfo->AggroTargetID == pLocalPC->pSpawn->SpawnID) {
			return "already primary target";
		}
		return "";
	}

	return "unsupported ability";
}

