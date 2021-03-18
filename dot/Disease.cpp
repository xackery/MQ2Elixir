#include "../MQ2Plugin.h"
#include "CommonDoTDisease.h"

bool CastDoTDisease() {
	PCHARINFO pChar = GetCharInfo();
	if (pChar->pSpawn->GetClass() != Shadowknight) return false;
	PSPAWNINFO pTargetSpawn = nullptr;
	if (pTarget) pTargetSpawn = (PSPAWNINFO)GetSpawnByID(pTarget->Data.SpawnID);

	PCHAR szNames[] = {

		"Heart Flutter", //36 24
		"Disease Cloud", //5 1
	};
	for (PCHAR szName : szNames) {
		if (HasBuff(pTargetSpawn, szName)) return false; //stop trying disease, we have a conflict
		if (!ActionCastSpell(szName)) continue;
		return true;
	}
}