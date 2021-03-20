#include "../MQ2Plugin.h"
#include "Core.h"
#include "Elixir.h"

using namespace std;

void Elixir::OnPulse()
{

	// Reset last pulse results
	isActionComplete = false;
	lastAction = "";

	for (int i = 0; i < 10; i++) {
		AttemptButton(i);
	}

	int maxGems = 8;
	if (PALTABILITY pAbility = AAByName("Mnemonic Retention")) {
		int rank = pAbility->CurrentRank - 1;
#if !defined(ROF2EMU) && !defined(UFEMU)
		if (((PcZoneClient*)pPCData)->HasAlternateAbility(pAbility->Index, NULL, false, false))
#else
		if (((PcZoneClient*)pPCData)->HasAlternateAbility(pAbility->Index, NULL, false))
#endif
		{
			rank++;
		}
		if (rank > 0 && rank < 10) {
			maxGems += rank;
		}
	}
	Gems[maxGems-1] = "reserved";

	for (int i = 0; i < maxGems-1; i++) {
		AttemptGem(i);
	}
	
	for (int i = maxGems; i < NUM_SPELL_GEMS; i++) {
		Gems[i] = "not available";
	}
	
	//TODO: buff cycle last gem (maxGem)
}

void Elixir::AttemptGem(int gemIndex)
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

	if (!Spell(gemIndex)) {
		Gems[gemIndex] = "spell: " + Gems[gemIndex];
		return;
	}

	lastAction = Gems[gemIndex];
	lastActionRepeatCooldown = (unsigned long)MQGetTickCount64() + 3000;
	lastGemIndex = gemIndex;
	isActionComplete = true;
	gemGlobalCooldown = (unsigned long)MQGetTickCount64() + 3000;
}