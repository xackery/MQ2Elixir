#include <mq/Plugin.h>
#include "Core.h"
#include "Elixir.h"

using namespace std;

void Elixir::OnPulse()
{
	PCHARINFO pChar = GetCharInfo();
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


	if (lastCastedSpellID > 0 && (!pCastSpellWnd || !GetCharInfo()->pSpawn->CastingData.IsCasting())) {
		lastCastedSpellID = 0;
	}

	if (lastCastedSpellID > 0 && pCastSpellWnd && !pSpellBookWnd->IsVisible() && GetCharInfo()->pSpawn->CastingData.IsCasting()) {
		if (lastCastedSpellID != GetCharInfo()->pSpawn->CastingData.SpellID) {
			lastCastedSpellID = 0;
		}
		if (lastCastedSpellID > 0 && !pTarget) {
			Execute("/stopcast");
			gemGlobalCooldown = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);
			LastAction = "stopping spell, target dead";
		}
	}
	
	if (pChar->pSpawn->GetClass() != Bard && IsMoving(pChar->pSpawn)) {
		movementGlobalCooldown = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
	}

	for (int i = 0; i < 12; i++) {
		ActionButton(i);
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
		ActionGem(i);
		
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
}

bool Elixir::IsHighHateAggro()
{
	if (!pAggroInfo) return false;
	if (pAggroInfo->aggroData[AD_Player].AggroPct >= HateAIMax) return true;
	PCHARINFO pChar = GetCharInfo();
	if (pAggroInfo->AggroTargetID == pChar->pSpawn->SpawnID) return true;

	return false;
}