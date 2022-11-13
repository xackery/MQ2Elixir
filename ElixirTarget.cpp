#include <mq/Plugin.h>
#include "Core.h"
#include "Elixir.h"

using namespace std;

// Target attempts to target the main assist
void Elixir::ActionTarget()
{
	if (gbInForeground && IsElixirDisabledOnFocus) {
		TargetAIStr = "AI frozen, has focus";
		return;
	}
    CHAR szTemp[MAX_STRING] = { 0 };
    if (!IsTargetAIRunning) {
		TargetAIStr = "AI not enabled";
        return;
    }

	if (!pLocalPC->Group) {
		TargetAIStr = "not in a group";
		return;
	}
	
    uint32_t assist_id = GetGroupMainAssistTargetID();
    if (!assist_id) {
		TargetAIStr = "no assist ID found";
        return;
    }

	SPAWNINFO* pAssist = GetSpawnByID(assist_id);
    if (!pAssist) {
		TargetAIStr = "spawn assist not found";
        return;
    }

	if (pTarget && pTarget->SpawnID == pAssist->SpawnID) {
		if (IsTargetPetAttack && GetCharInfo()->pSpawn->PetID > 0 && PetTargetID() == 0 && pAssist->SpawnID != GetCharInfo()->pSpawn->PetID) {
			EzCommand("/pet attack");
			TargetAIStr = "pet attacking";
			return;
		}

		if (IsTargetAutoAttack && !pEverQuestInfo->bAutoAttack) {
			EzCommand("/attack");
			TargetAIStr = "auto attacking";
			return;
		}
		TargetAIStr = "assisting";
		return;
	}

	if (pLocalPlayer->Animation == 16) {
		TargetAIStr = "animation 16 (feigndeath), ignoring targetting";
		return;
	}

	if (IsMoving(pCharSpawn)) {
		TargetAIStr = "I am moving";
		return;
	}

	if (DistanceToSpawn(pCharSpawn, pAssist) > TargetAIMinRange) {
        sprintf_s(szTemp, "target > %d%%", TargetAIMinRange);
		TargetAIStr = szTemp;
		return;
	}

	if (!LineOfSight(pCharSpawn, pAssist)) {
		TargetAIStr = "not line of sight";
		return;
	}

	if (pAssist->Type != SPAWN_NPC) {
		TargetAIStr = "assist target not NPC";
		return;
	}

	if (lastTargetRepeatCooldownTimer > std::chrono::steady_clock::now()) {
		TargetAIStr = "timer delay to change";
		return;
	}

	lastTargetRepeatCooldownTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(6000);
    
	pTarget = pAssist;
	return;
}
