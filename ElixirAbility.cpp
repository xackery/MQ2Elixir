#include "../MQ2Plugin.h"
#include "Core.h"
#include "Elixir.h"

using namespace std;


bool isBashAllowed()
{
	if (!GetCharInfo2()) return false;
	if (GetCharInfo2()->pInventoryArray->Inventory.Secondary && GetItemFromContents(GetCharInfo2()->pInventoryArray->Inventory.Secondary)->ItemType == 8) return true;
#if !defined(ROF2EMU) && !defined(UFEMU)
	if (GetCharInfo2()->Class == EQData::Warrior && IsAAPurchased("Two-Handed Bash")) return true;
	if (GetCharInfo2()->Class == EQData::Paladin || GetCharInfo2()->Class == EQData::Shadowknight && IsAAPurchased("Improved Bash")) return true;
#else
	if (IsAAPurchased("2 Hand Bash")) return true;
#endif
	return false;
}

// Ability returns a string with a reason if provided ability cannot be used
std::string Elixir::Ability(int abilityIndex)
{

	PCHARINFO pChar = (PCHARINFO)pCharData;
	if (!pChar) {
		return "char not loaded";
	}

	if (!pLocalPlayer) {
		return "pLocalPlayer not loaded";
	}

	DWORD nToken = pCSkillMgr->GetNameToken(abilityIndex);
	if (!HasSkill(abilityIndex)) {
		return "don't have ability";
	}
	char* abilityName = pStringTable->getString(nToken, 0);
	if (!abilityName) {
		return "can't find ability";
	}
	if (!pCSkillMgr->IsActivatedSkill(abilityIndex)) {
		return "not activated skill (ignored)";
	}

	if (!pCSkillMgr->IsAvailable(abilityIndex)) {
		return "on cooldown";
	}

	if (stricmp(abilityName, "Hide") == 0) {
		return "(ignored)";
	}

	// Do logical ability checks


	if (!pTarget) {
		return "no target";
	}

	if (Distance3DToSpawn(pChar->pSpawn, (PSPAWNINFO)pTarget) > 16) {
		return "too far away";
	}

	if (pTarget && !pCharSpawn->CanSee((EQPlayer*)pTarget)) {
		return "target not line of sight";
	}

	if (pTarget->Data.Type != SPAWN_NPC) {
		return "non npc targetted";
	}


	if (SpawnPctHPs((PSPAWNINFO)pTarget) > 99) {
		return "target > 99%";
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
		if (Distance3DToSpawn(pChar->pSpawn, (PSPAWNINFO)pTarget) > 14) {
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
		if (GetCharInfo()->pGroupInfo == nullptr) {
			return "not in a group";
		}

		if (!pAggroInfo) {
			return "pAggroInfo empty";
		}

		bool isMainTank = false;
		GROUPMEMBER* pG;
		PSPAWNINFO pSpawn;
		for (int i = 0; i < 6; i++) {
			pG = GetCharInfo()->pGroupInfo->pMember[i];
			if (!pG) continue;
			if (pG->Offline) continue;
			//if (${Group.Member[${i}].Mercenary}) /continue
			//if (${ Group.Member[${i}].OtherZone }) / continue
			pSpawn = pG->pSpawn;
			if (!pSpawn) continue;
			if (pSpawn->SpawnID != pChar->pSpawn->SpawnID) continue;
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

		if (pAggroInfo->AggroTargetID == pChar->pSpawn->SpawnID) {
			return "already primary target";
		}
		return "";
	}

	return "unsupported ability";
}

