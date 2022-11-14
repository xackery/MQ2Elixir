#include <mq/Plugin.h>
#include "Core.h"
#include "Elixir.h"

using namespace std;

void Elixir::ActionGem(int gemIndex)
{
	if (gbInForeground && IsElixirDisabledOnFocus) {
		Gems[gemIndex] = "AI frozen, has focus";
		return;
	}
	

	if (!pLocalPlayer) {
		Gems[gemIndex] = "pLocalPlayer not loaded";
		return;
	}

	if (ZoneCooldownTimer > std::chrono::steady_clock::now()) {
		Gems[gemIndex] = "zone cooldown";
		return;
	}

	if (isActionComplete) {
		Gems[gemIndex] = "earlier action completed";
		return;
	}

	if (pLocalPlayer->GetClass() != Bard && IsMoving(pLocalPlayer)) {
		Gems[gemIndex] = "player is moving";
		return;
	}

	if (gemGlobalCooldown > std::chrono::steady_clock::now()) {
		Gems[gemIndex] = "gem AI cooldown";
		return;
	}

	if (movementGlobalCooldown > std::chrono::steady_clock::now()) {
		Gems[gemIndex] = "movement AI cooldown";
		return;
	}

	if (AreObstructionWindowsVisible()) {
		Gems[gemIndex] = "obstruction window visible";
		return;
	}

	if (pLocalPlayer->CastingData.IsCasting()) {
		Gems[gemIndex] = "already casting";
		return;
	}

	if (pLocalPC->Stunned) {
		Gems[gemIndex] = "player stunned";
		return;
	}

	if (pLocalPlayer->HideMode) {
		Gems[gemIndex] = "player invisible";
		return;
	}


	if ((int)((CDisplay*)pDisplay)->TimeStamp <= ((PSPAWNINFO)pLocalPlayer)->GetSpellCooldownETA()) {
		Gems[gemIndex] = "all gems on recent use cooldown";
		return;
	}

	if (((CDisplay*)pDisplay)->TimeStamp <= ((PSPAWNINFO)pLocalPlayer)->SpellGemETA[gemIndex]) {
		Gems[gemIndex] = "gem on cooldown";
		return;
	}

	if (lastGemIndex == gemIndex && lastActionRepeatCooldown > std::chrono::steady_clock::now()) {
		Gems[gemIndex] = "last action repeat cooldown";
		return;
	}


	int64_t spellID = GetMemorizedSpell(gemIndex);
	if (!spellID) {
		Gems[gemIndex] = "no spell memorized";
		return;
	}

	PSPELL pSpell = GetSpellByID(spellID);
	if (!pSpell) {
		Gems[gemIndex] = "spell not found";
		return;
	}

	Gems[gemIndex] = Spell(pSpell);
	if (strlen(Gems[gemIndex].c_str()) > 0) {
		return;
	}

	unsigned long stunDuration = StunDuration(pSpell);
	if (stunDuration > 1000) {
		stunGlobalCooldown = std::chrono::steady_clock::now() + std::chrono::milliseconds(stunDuration);
	}

	ActionCastGem(gemIndex + 1);
	Gems[gemIndex] = "casting";
	LastAction = "gem " + to_string(gemIndex + 1) + " " + Gems[gemIndex];
	lastActionRepeatCooldown = std::chrono::steady_clock::now() + std::chrono::milliseconds(3000);
	lastGemIndex = gemIndex;
	if (pSpell->TargetType == 5) { //Single
		lastCastedSpellID = pSpell->ID;
	}
	isActionComplete = true;
	gemGlobalCooldown = std::chrono::steady_clock::now() + std::chrono::milliseconds(3000);
}