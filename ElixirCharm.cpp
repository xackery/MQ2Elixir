#include <mq/Plugin.h>
#include "Core.h"
#include "Elixir.h"

using namespace std;


void Elixir::ClearCharmTarget() 
{
	CharmSpawnID = 0;
	sprintf_s(CharmName, "No Target");
}

void Elixir::ActionCharm()
{

	if (gbInForeground && IsElixirDisabledOnFocus) {
		CharmAIStr = "AI frozen, has focus";
		return;
	}

	if (!IsCharmAIRunning) {
		CharmAIStr = "AI not enabled";
		if (IsCharmCurrentValid) IsCharmCurrentValid = false;
		return;
	}

	if (ZoneCooldownTimer > std::chrono::steady_clock::now()) {
		CharmAIStr = "zone cooldown";
		return;
	}

	if (charmCooldownTimer > std::chrono::steady_clock::now()) {
		CharmAIStr = "charm on cooldown";
		return;
	}

	// Used for UI caching
	if (IsCharmCurrentValid && (!pTarget || pTarget->Type != SPAWN_NPC)) {
		IsCharmCurrentValid = false;
	}
	if (!IsCharmCurrentValid && pTarget && pTarget->Type == SPAWN_NPC) {
		IsCharmCurrentValid = true;
	}

	if (CharmSpawnID == 0) {
		CharmAIStr = "no charm target set";
		return;
	}

	if (isActionComplete) {
		CharmAIStr = "earlier action completed";
		return;
	}

	PlayerClient* pSpawn = GetSpawnByID(CharmSpawnID);
	if (!pSpawn) {
		ClearCharmTarget();
		CharmAIStr = "charm target no longer valid";
		return;
	}

	if (pSpawn->Type != SPAWN_NPC) {
		ClearCharmTarget();
		CharmAIStr = "charm target not an NPC";
		return;
	}

	if (pLocalPlayer->PetID > 0) {
		if (pLocalPlayer->PetID != CharmSpawnID) {
			CharmAIStr = "currently have another pet";
			return;
		}
		CharmAIStr = "proper pet charmed";
		return;
	}

	if (DistanceToSpawn(pLocalPlayer, pSpawn) > TargetAIMinRange) {
		CharmAIStr = "target too far";
		return;
	}

	if (!LineOfSight(pLocalPlayer, pSpawn)) {
		CharmAIStr = "not line of sight";
		return;
	}

	CHAR szTemp[MAX_STRING] = { 0 };


	if (pLocalPC->Stunned) {
		CharmAIStr = "player stunned";
		return;
	}

	if (AreObstructionWindowsVisible()) {
		CharmAIStr = "obstruction window visible";
		return;
	}


	if (IsMoving(pLocalPlayer)) {
		charmCooldownTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
		CharmAIStr = "currently moving";
		return;
	}

	if (!IsCastingReady()) {
		charmCooldownTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
		CharmAIStr = "currently casting";
		return;
	}


	if (pLocalPlayer->Animation == 16) {
		CharmAIStr = "animation 16 (feigndeath)";
		return;
	}

	int gemIndex = -1;
	PSPELL pSpell = nullptr;
	// we used a gem last time
	if (lastCharmGemIndex > -1) {
		int64_t spellID = GetMemorizedSpell(lastCharmGemIndex);
		if (spellID) {
			pSpell = GetSpellByID(spellID);
			// check if spell changed since last use on gem
			if (pSpell && pSpell->ID == lastCharmSpellID) {
				CharmAIStr = "trying to charm with last charm gem used";
				gemIndex = lastCharmGemIndex;
			}
		}
	}

	if (gemIndex == -1) {
		for (int i = 0; i < maxGemCount; i++) {
			if (IgnoreGems[i]) continue;

			int spellID = GetMemorizedSpell(i);
			if (!spellID) {
				continue;
			}
			pSpell = GetSpellByID(spellID);
			if (!pSpell) {
				continue;
			}
			if (!IsCharm(pSpell)) {
				continue;
			}
			lastCharmGemIndex = i;
			lastCharmSpellID = spellID;
			CharmAIStr = "trying to charm";
			gemIndex = i;
			break;
		}
	}

	if (gemIndex == -1) {
		CharmAIStr = "no charm spell found";
		return;
	}

	if (pSpell != nullptr && GetPcProfile()->Mana < (int)pSpell->ManaCost) {
		CharmAIStr = "not enough mana (" + to_string((int)pSpell->ManaCost) + "/" + to_string(GetPcProfile()->Mana) + ")";
		return;
	}
	
	if (!pTarget || pTarget->SpawnID != CharmSpawnID) {		
		pTarget = pSpawn;
	}
	ActionCastGem(gemIndex + 1);
	LastAction = "Charm AI casting gem";
	charmCooldownTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
	isActionComplete = true;
	return;
}