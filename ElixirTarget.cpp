#include <mq/Plugin.h>
#include "Core.h"
#include "Elixir.h"

using namespace std;

// Target attempts to target the main assist
void Elixir::ActionTarget()
{
    CHAR szTemp[MAX_STRING] = { 0 };
    if (!IsTargetAIRunning) {
		TargetStr = "AI not enabled";
        return;
    }

	if (!pLocalPC->Group) {
		TargetStr = "not in a group";
		return;
	}
	
    uint32_t assist_id = GetGroupMainAssistTargetID();
    if (!assist_id) {
		TargetStr = "no assist ID found";
        return;
    }

	SPAWNINFO* pAssist = GetSpawnByID(assist_id);
    if (!pAssist) {
		TargetStr = "spawn assist not found";
        return;
    }

	if (pTarget && pTarget->SpawnID == pAssist->SpawnID) {
		TargetStr = "assisting";
		return;
	}

	if (pLocalPlayer->Animation == 16) {
		TargetStr = "animation 16 (feigndeath), ignoring targetting";
		return;
	}

	if (IsMoving(pCharSpawn)) {
		TargetStr = "I am moving";
		return;
	}

	if (DistanceToSpawn(pCharSpawn, pAssist) > TargetAIMinRange) {
        sprintf_s(szTemp, "target > %d%%", TargetAIMinRange);
		TargetStr = szTemp;
		return;
	}

	if (!LineOfSight(pCharSpawn, pAssist)) {
		TargetStr = "not line of sight";
		return;
	}

	if (lastTargetRepeatCooldownTimer > std::chrono::steady_clock::now()) {
		TargetStr = "timer delay to change";
		return;
	}

	lastTargetRepeatCooldownTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(6000);
    
	pTarget = pAssist;
	return;
}
