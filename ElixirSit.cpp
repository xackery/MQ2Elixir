#include <mq/Plugin.h>
#include "Core.h"
#include "Elixir.h"

using namespace std;

// Try to sit down
void Elixir::ActionSit()
{
	if (gbInForeground && IsElixirDisabledOnFocus) {
		MeditateAIStr = "AI frozen, has focus";
		return;
	}
	if (!IsMeditateAIRunning) {
		MeditateAIStr = "AI not enabled";
		return;
	}

	if (!IsMeditateBySitting) {
		MeditateAIStr = "AI by sitting not enabled";
		return;
	}
	CHAR szTemp[MAX_STRING] = { 0 };

	if (ZoneCooldownTimer > std::chrono::steady_clock::now()) {
		MeditateAIStr = "zone cooldown";
		return;
	}

	if (pLocalPC->Stunned) {
		MeditateAIStr = "player stunned";
		return;
	}

	if (AreObstructionWindowsVisible()) {
		MeditateAIStr = "obstruction window visible";
		return;
	}

	if (DoIHaveHate()) {
		MeditateAIStr = "currently have aggro";
		return;
	}

	if (sitCooldownTimer > std::chrono::steady_clock::now()) {
		MeditateAIStr = "sit on cooldown";
		return;
	}
	
	if (pLocalPC->standstate == STANDSTATE_SIT) {
		if (sitCooldownTimer <= std::chrono::steady_clock::now() && PctMana() >= 99) {
			//MeditateAIStr = "standing up, full mana";
			//EzCommand("/stand");
			//return;
		}
		sprintf_s(szTemp, "currently sitting (%d%%)", PctMana());
		MeditateAIStr = szTemp;
		
		lastHPOnSit = SpawnPctHPs(pLocalPlayer);
		return;
	}

	if (PctMana() >= 99) {
		MeditateAIStr = "full mana";
		return;
	}

	if (IsMoving(pLocalPlayer)) {
		sitCooldownTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(6000);
		MeditateAIStr = "currently moving";
		return;
	}


	if (!IsCastingReady()) {
		sitCooldownTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(6000);
		MeditateAIStr = "currently casting";
		return;
	}

	if (pLocalPC->standstate != STANDSTATE_STAND) {
		MeditateAIStr = "not standing";
		return;
	}

	if (CombatState() == COMBATSTATE_COMBAT) {
		if (!IsMeditateEnabledDuringCombat) {
			MeditateAIStr = "combat detected, meditate disabled";
		}
		if (lastHPOnSit > SpawnPctHPs(pLocalPlayer) + 5) {
			sitCooldownTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(12000);
		}
		else {
			sitCooldownTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(6000);
		}
	}
	else {
		sitCooldownTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
	}

	lastHPOnSit = SpawnPctHPs(pLocalPlayer);
	EzCommand("/sit");
	MeditateAIStr = "trying to sit";
	return;
}