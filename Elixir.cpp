#include "../MQ2Plugin.h"
#include "Core.h"
#include "Elixir.h"

using namespace std;

void Elixir::OnPulse()
{

	// Reset last pulse results
	isActionComplete = false;
	lastGemIndex = -1;

	//GetCharInfo()->pSpawn->CastingData.SpellETA <= 0
	//if (!pCastSpellWnd && GetCharInfo()->pSpawn->CastingData.IsCasting() && GetCharInfo()->pSpawn->CastingData.SpellID) {
	int eta = ((PSPAWNINFO)pLocalPlayer)->CastingData.SpellETA;
	if (eta > 0) eta = ((PSPAWNINFO)pLocalPlayer)->CastingData.SpellETA - ((PSPAWNINFO)pLocalPlayer)->TimeStamp;

	if (pCastSpellWnd && !pSpellBookWnd->IsVisible() && GetCharInfo()->pSpawn->CastingData.IsCasting() && eta <= 0) {
		Execute("/stopsong");
		LastAction = "stopping bard song";
	}

	for (int i = 0; i < 12; i++) {
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
		
		if (LONG spellID = GetMemorizedSpell(i)) {			
			if (PSPELL pSpell = GetSpellByID(spellID)) {
				char result[100];
				sprintf(result, "CSPW_Spell%d", i);
				if (CButtonWnd* pGem = (CButtonWnd*)pCastSpellWnd->GetChildItem(result)) {
					char result[100];
					sprintf(result, "%d) %s - %s", i+1, pSpell->Name, Gems[i].c_str());
					pGem->SetTooltip(result);
				}
			}
		}

		
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

	if (!Spell(gemIndex)) {
		Gems[gemIndex] = "spell: " + Gems[gemIndex];
		return;
	}

	LastAction = "gem " + to_string(gemIndex+1) + " " + Gems[gemIndex];
	lastActionRepeatCooldown = (unsigned long)MQGetTickCount64() + 3000;
	lastGemIndex = gemIndex;
	isActionComplete = true;
	gemGlobalCooldown = (unsigned long)MQGetTickCount64() + 3000;
}