#include "../MQ2Plugin.h"
#include "Core.h"
#include "Elixir.h"

using namespace std;

// Button attempts to press the provided button index
void Elixir::ActionButton(int buttonIndex)
{
	CHAR szTemp[MAX_STRING] = { 0 };
	CInvSlotWnd* invSlot = nullptr;
	PSPELL pSpell = nullptr;
	PITEMINFO pItem = nullptr;
	PCONTENTS pCont = nullptr;
	CXWnd* spellGem = nullptr;
	int gemIndex;

	if (isActionComplete) {
		Buttons[buttonIndex] = "earlier action completed";
		return;
	}

	PCHARINFO pChar = GetCharInfo();
	if (!pChar) {
		Buttons[buttonIndex] = "char not loaded";
		return;
	}

	if (ZoneCooldown > MQGetTickCount64()) {
		Buttons[buttonIndex] = "zone cooldown";
		return;
	}


	if (pChar->pSpawn->GetClass() != Bard && isActionComplete) {
		Buttons[buttonIndex] = "earlier action complete";
		return;
	}

	if (GetCharInfo()->Stunned) {
		Buttons[buttonIndex] = "player stunned";
		return;
	}

	if (GetCharInfo()->pSpawn->HideMode) {
		Buttons[buttonIndex] = "player invisible";
	}

	if (lastButtonIndex == buttonIndex && lastActionRepeatCooldown > MQGetTickCount64()) {
		Buttons[buttonIndex] = "last action repeat cooldown";
		return;
	}


	if (!pHotButtonWnd) {
		Buttons[buttonIndex] = "button window not found";
		return;
	}

	sprintf(szTemp, "HB_Button%d", buttonIndex + 1);
	CButtonWnd* hbtnRootWnd = (CButtonWnd*)pHotButtonWnd->GetChildItem(szTemp);
	if (!hbtnRootWnd) {
		Buttons[buttonIndex] = "no hot button root";
		return;
	}

	//UINT timeRemaining = hbtnRootWnd->GetCoolDownTimeRemaining();
	//UINT totalDuration = hbtnChild->GetCoolDownTotalDuration();
	
	
	CXWnd* hbtnChildXWnd = hbtnRootWnd->GetChildItem(szTemp);
	CButtonWnd* hbtnChildWnd = (CButtonWnd*)hbtnChildXWnd;
	if (!hbtnChildWnd) {
		Buttons[buttonIndex] = "no hot button child";
		return;
	}
	
	


	CHotButton* hbtn = (CHotButton*)hbtnChildWnd;
	if (!hbtn) {
		Buttons[buttonIndex] = "no hot button";
		return;
	}

	//?? on abilities, timeremaining > 0 means it's available.
	/*if (hbtn->LastButtonType != 9  && timeRemaining > 0) {
		Buttons[buttonIndex] = "time remaining > 0";
		return;
	}*/

	switch (hbtn->LastButtonType) {
	case 0: //Empty
		Buttons[buttonIndex] = "empty";
		return;
	case 1: //Melee Attack, Ranged Attack
		Buttons[buttonIndex] = "melee";
		return;
	case 4: //AA, social button
		Buttons[buttonIndex] = "aa";
		return;
	case 7: //Spell gem
		spellGem = hbtnRootWnd->GetChildItem("HB_SpellGem");
		if (!spellGem) {
			Buttons[buttonIndex] = "gem not found";
			return;
		}
		if (!spellGem->IsVisible()) {
			Buttons[buttonIndex] = "gem invisible";
			return;
		}

		GetCXStr(spellGem->GetTooltip(), szTemp, MAX_STRING);
		if (strlen(szTemp) == 0) {
			Buttons[buttonIndex] = "gem empty";
			return;
		}

		pSpell = GetSpellByName(szTemp);
		if (!pSpell) {
			Buttons[buttonIndex] = "spell not found";
			return;
		}

		gemIndex = -1;
		for (int GEM = 0; GEM < NUM_SPELL_GEMS; GEM++) {
			if (GetMemorizedSpell(GEM) != pSpell->ID) continue;
			gemIndex = GEM;
			break;
		}
		if (gemIndex == -1) {
			Buttons[buttonIndex] = "spell gem not found";
			return;
		}
		Buttons[buttonIndex] = Spell(pSpell);
		if (Buttons[buttonIndex].c_str() > 0) {
			return;
		}

		ActionGem(gemIndex);
		Buttons[buttonIndex] = "casting";
		LastAction = "button " + to_string(buttonIndex + 1) + " " + Buttons[buttonIndex] + " gem " + to_string(gemIndex + 1);
		lastActionRepeatCooldown = (unsigned long)MQGetTickCount64() + 3000;
		lastGemIndex = gemIndex;
		isActionComplete = true;
		gemGlobalCooldown = (unsigned long)MQGetTickCount64() + 3000;
		return;
	case 9: //Ability
		invSlot = (CInvSlotWnd*)hbtnChildWnd->GetChildItem("HB_InvSlot");
		if (!invSlot) {
			Buttons[buttonIndex] = "item slot not found";
			return;
		}

		GetCXStr(invSlot->WindowText, szTemp, MAX_STRING);
		if (strlen(szTemp) == 0) {
			Buttons[buttonIndex] = "ability empty";
			return;
		}

		for (int i = 0; i < NUM_SKILLS; i++) {
			DWORD nToken = pCSkillMgr->GetNameToken(i);
			if (!HasSkill(i)) continue;
			char* abilityName = pStringTable->getString(nToken, 0);
			if (!abilityName) continue;

			Buttons[buttonIndex] = Ability(i);
			if (strlen(Buttons[buttonIndex].c_str()) > 0) {
				return;
			}
			DoAbility(pChar->pSpawn, szTemp);
			LastAction = "pressing hotbutton " + buttonIndex;
			Buttons[buttonIndex] = LastAction;
			lastActionRepeatCooldown = (unsigned long)MQGetTickCount64() + 3000;
			lastButtonIndex = buttonIndex;
			isActionComplete = true;
			return;
		}

		Buttons[buttonIndex] = "ability not found";
		return;
	case 5: //Equipment
	case 6: // Generic actions (sit, spell, etc)
		Buttons[buttonIndex] = "generic action (ignored)";
		return;
	case 12: //Item
		invSlot = (CInvSlotWnd*)hbtnChildWnd->GetChildItem("HB_InvSlot");
		if (!invSlot) {
			Buttons[buttonIndex] = "item slot not found";
			return;
		}

		if (!invSlot->IsVisible()) {
			Buttons[buttonIndex] = "item slot not visible";
			return;
		}

		if (!invSlot->pEQInvSlot) {
			Buttons[buttonIndex] = "item slot sub not found";
			return;
		}

		pCont = 0;
		invSlot->pEQInvSlot->GetItemBase(&pCont);
		if (!pCont) {
			Buttons[buttonIndex] = "no contents in item";
			return;
		}

		pItem = GetItemFromContents(pCont);
		if (!pItem) {
			Buttons[buttonIndex] = "no item via content";
			return;
		}

		if (pItem->Clicky.TimerID != 0xFFFFFFFF && GetItemTimer(pCont) > 0) {
			Buttons[buttonIndex] = "on cooldown";
			return;
		}

		if (pItem->Type == ITEMTYPE_NORMAL && pCont->Charges <= 0) {
			Buttons[buttonIndex] = "normal item (no clicky)";
			return;
		}

		pSpell = GetSpellByID(GetItemFromContents(pCont)->Clicky.SpellID);
		if (!pSpell) {
			Buttons[buttonIndex] = "no clicky spell";
			return;
		}

		if (strlen(pSpell->Name) == 0) {
			Buttons[buttonIndex] = "unknown clicky spell";
			return;
		}

		Buttons[buttonIndex] = Spell(pSpell);
		if (strlen(Buttons[buttonIndex].c_str()) > 0) {
			return;
		}
		Buttons[buttonIndex] = "TODO: click item";


		//must be equipped logic?
	//	PCONTENTS pEquipped = EquippedSlot(pCont));
//		if (!pEquipped) {

	//	}
		return;
	case 10:
	case 15: //Combat Ability
		Buttons[buttonIndex] = "combat ability";
		invSlot = (CInvSlotWnd*)hbtnChildWnd->GetChildItem("HB_InvSlot");
		if (!invSlot) {
			Buttons[buttonIndex] = "item slot not found";
			return;
		}
		
		GetCXStr(invSlot->WindowText, szTemp, MAX_STRING);
		if (strlen(szTemp) == 0) {
			Buttons[buttonIndex] = "combat ability empty";
			return;
		}
		Buttons[buttonIndex] = szTemp;
		return;
	default:
		sprintf(szTemp, "unkown button type: %i", hbtn->LastButtonType);
		Buttons[buttonIndex] = szTemp;
		return;
	}

	return;






	//TODO: attempt to click item
	if (isActionComplete) {
		LastAction = "pressing hotbutton " + buttonIndex;
		Buttons[buttonIndex] = LastAction;
		lastActionRepeatCooldown = (unsigned long)MQGetTickCount64() + 3000;
		lastButtonIndex = buttonIndex;
		isActionComplete = true;
		return;
	}

	Buttons[buttonIndex] = "unrecognized button";
	return;
}
