#include <mq/Plugin.h>
#include "Core.h"
#include "Elixir.h"

using namespace std;

void Elixir::OnPulse()
{
	// Reset last pulse results
	isActionComplete = false;
	lastGemIndex = -1;
	

	//pLocalPlayer->CastingData.SpellETA <= 0
	//if (!pCastSpellWnd && pLocalPlayer->CastingData.IsCasting() && pLocalPlayer->CastingData.SpellID) {
	int eta = ((PSPAWNINFO)pLocalPlayer)->CastingData.SpellETA;
	if (eta > 0) eta = ((PSPAWNINFO)pLocalPlayer)->CastingData.SpellETA - ((PSPAWNINFO)pLocalPlayer)->TimeStamp;

	if (pCastSpellWnd && !pSpellBookWnd->IsVisible() && pLocalPlayer->CastingData.IsCasting() && eta <= 0) {
		Execute("/stopsong");
		LastAction = "stopping bard song";
	}


	if (lastCastedSpellID > 0 && (!pCastSpellWnd || !pLocalPlayer->CastingData.IsCasting())) {
		lastCastedSpellID = 0;
	}

	if (lastCastedSpellID > 0 && pCastSpellWnd && !pSpellBookWnd->IsVisible() && pLocalPlayer->CastingData.IsCasting()) {
		if (lastCastedSpellID != pLocalPlayer->CastingData.SpellID) {
			lastCastedSpellID = 0;
		}
		if (lastCastedSpellID > 0 && !pTarget) {
			Execute("/stopcast");
			gemGlobalCooldown = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
			LastAction = "stopping spell, target dead";
		}
	}
	
	if (pLocalPlayer->GetClass() != Bard && IsMoving(pLocalPlayer)) {
		movementGlobalCooldown = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
	}

	// higher priority logic
	ActionCharm();
	
	// normal logic
	for (int i = 0; i < 12; i++) {
		ActionButton(i);
	}

	int maxGems = 8;
	if (PALTABILITY pAbility = AAByName("Mnemonic Retention")) {
		int rank = pAbility->CurrentRank - 1;
		if (((PcZoneClient*)pPCData)->HasAlternateAbility(pAbility->Index, NULL, false))
		{
			rank++;
		}
		if (rank > 0 && rank < 10) {
			maxGems += rank;
		}
	}
	maxGemCount = maxGems;

	ignoreGemCount = 0;
	for (int i = 0; i < maxGems; i++) {
		if (IgnoreGems[i]) {
			Gems[i] = "ignored gem";
			ignoreGemCount++;
		}
		else {
			ActionGem(i);
		}
		
		if (int64_t spellID = GetMemorizedSpell(i)) {			
			if (PSPELL pSpell = GetSpellByID(spellID)) {
				char result[100];
				sprintf_s(result, "CSPW_Spell%d", i);
				if (CButtonWnd* pGem = (CButtonWnd*)pCastSpellWnd->GetChildItem(result)) {
					char result[100];
					sprintf_s(result, "%d) %s - %s", i+1, pSpell->Name, Gems[i].c_str());
					pGem->SetTooltip(result);
				}
			}
		}

		
	}

	for (int i = maxGems; i < NUM_SPELL_GEMS; i++) {
		Gems[i] = "not available";
	}
	ActionTarget();
	ActionSit();
}

bool Elixir::IsHighHateAggro()
{
	if (!pAggroInfo) return false;
	if (pAggroInfo->aggroData[AD_Player].AggroPct >= HateAIMax) return true;
	if (pAggroInfo->AggroTargetID == pLocalPlayer->SpawnID) return true;

	return false;
}