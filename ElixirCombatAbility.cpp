#include "../MQ2Plugin.h"
#include "Core.h"
#include "Elixir.h"

using namespace std;

// Spell returns a string with a reason if provided spell cannot be cast
std::string Elixir::CombatAbility(PSPELL pSpell)
{
	unsigned long timeNow = (unsigned long)time(NULL);
#if !defined(ROF2EMU) && !defined(UFEMU)
	if (pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex, pSpell->SpellGroup) > timeNow) {
#else
	if (pSpell->ReuseTimerIndex != -1 && pSpell->ReuseTimerIndex < 20 && pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex) > timeNow) {
#endif
		return "on cooldown";
	}

	//If substring "Discipline" is found in the name of the disc, this is an active disc. Let's see if there is already one running.
	if (!strstr(pSpell->Name, "Discipline")) {
		return Spell(pSpell);
	}

	if (!pCombatAbilityWnd) {
		return "combat window not found";
	}

	CXWnd* child = ((CXWnd*)pCombatAbilityWnd)->GetChildItem("CAW_CombatEffectLabel");
	if (!child) {
		return "combat window effect not found";
	}
	char szBuffer[2048] = { 0 };
	if (!GetCXStr(child->CGetWindowText(), szBuffer, MAX_STRING)) {
		return "combat window effect text not found";
	}

	if (szBuffer[0] != '\0') {
		return "another discipline active";
	}

	return Spell(pSpell);
}