#include "../MQ2Plugin.h"
#include "Core.h"
#include "Elixir.h"

using namespace std;

void Elixir::ActionGem(int gemIndex)
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) {
		Gems[gemIndex] = "char not loaded";
		return;
	}

	if (!pLocalPlayer) {
		Gems[gemIndex] = "pLocalPlayer not loaded";
		return;
	}

	if (ZoneCooldown > MQGetTickCount64()) {
		Gems[gemIndex] = "zone cooldown";
		return;
	}

	if (isActionComplete) {
		Gems[gemIndex] = "earlier action completed";
		return;
	}

	if (pChar->pSpawn->GetClass() != Bard && IsMoving(pChar->pSpawn)) {
		Gems[gemIndex] = "player is moving";
		return;
	}

	if (gemGlobalCooldown > MQGetTickCount64()) {
		Gems[gemIndex] = "gem AI cooldown";
		return;
	}

	if (AreObstructionWindowsVisible()) {
		Gems[gemIndex] = "obstruction window visible";
		return;
	}

	if (GetCharInfo()->pSpawn->CastingData.IsCasting()) {
		Gems[gemIndex] = "already casting";
		return;
	}

	if (GetCharInfo()->Stunned) {
		Gems[gemIndex] = "player stunned";
		return;
	}

	if (GetCharInfo()->pSpawn->HideMode) {
		Gems[gemIndex] = "player invisible";
		return;
	}


	if ((int)((PCDISPLAY)pDisplay)->TimeStamp <= ((PSPAWNINFO)pLocalPlayer)->GetSpellCooldownETA()) {
		Gems[gemIndex] = "all gems on recent use cooldown";
		return;
	}

	if (((PCDISPLAY)pDisplay)->TimeStamp <= ((PSPAWNINFO)pLocalPlayer)->SpellGemETA[gemIndex]) {
		Gems[gemIndex] = "gem on cooldown";
		return;
	}

	if (lastGemIndex == gemIndex && lastActionRepeatCooldown > MQGetTickCount64()) {
		Gems[gemIndex] = "last action repeat cooldown";
		return;
	}


	LONG spellID = GetMemorizedSpell(gemIndex);
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

	ActionCastGem(gemIndex + 1);
	Gems[gemIndex] = "casting";
	LastAction = "gem " + to_string(gemIndex + 1) + " " + Gems[gemIndex];
	lastActionRepeatCooldown = (unsigned long)MQGetTickCount64() + 3000;
	lastGemIndex = gemIndex;
	isActionComplete = true;
	gemGlobalCooldown = (unsigned long)MQGetTickCount64() + 3000;
}