#include <mq/Plugin.h>
#include "Core.h"
#include "Elixir.h"

using namespace std;

// Spell returns a string with a reason if provided spell cannot be cast
std::string Elixir::CombatAbility(PSPELL pSpell)
{
	unsigned long timeNow = (unsigned long)time(NULL);
	if (pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex, pSpell->SpellGroup) > timeNow) {
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

	if (!child->GetWindowText().empty()) {
		return "another discipline is active";
	}

	return Spell(pSpell);
}