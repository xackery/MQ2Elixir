#include "../MQ2Plugin.h"
#include "Core.h"
#include "Elixir.h"

using namespace std;

void Elixir::AttemptButton(int buttonIndex)
{
	CHAR szTemp[MAX_STRING] = { 0 };

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

	CXWnd* wnd = FindMQ2Window("HotButtonWnd");
	if (!wnd) {
		Buttons[buttonIndex] = "button window not found";
		return;
	}

	sprintf(szTemp, "HB_Button%d", buttonIndex + 1);
	CXWnd* child = wnd->GetChildItem(szTemp);
	if (!child) {
		Buttons[buttonIndex] = "button area not found";
		return;
	}

	CHotButton* hbtn = (CHotButton*)child;
	if (!hbtn) {
		Buttons[buttonIndex] = "button not set";
	}

	CXWnd* child2 = child->GetChildItem(szTemp);
	/*if (child2 && child2->IsVisible()) {

		Buttons[buttonIndex] = "found ability?";
		return;
	}
	*/

	child2 = child->GetChildItem("HB_InvSlot");
	if (child2 && child2->IsVisible()) {
		GetCXStr(child2->GetTooltip(), szTemp, MAX_STRING);
		//sprintf(szTemp, "item: %s", szTemp);
		Buttons[buttonIndex] = szTemp;
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
	}

	child2 = child->GetChildItem("HB_SpellGem");
	if (child2 && child2->IsVisible()) {
		GetCXStr(child2->GetTooltip(), szTemp, MAX_STRING);
		//sprintf(szTemp, "spell gem: %s", szTemp);
		//Buttons[buttonIndex] = child2->Tooltip->Text;
		Buttons[buttonIndex] = szTemp;
		return;
		//TODO: iterate gems, figure out which one is this hotbutton, and AttemptGem(gemIndex);
		if (isActionComplete) {
			LastAction = "pressing hotbutton " + buttonIndex;
			Buttons[buttonIndex] = LastAction;
			lastActionRepeatCooldown = (unsigned long)MQGetTickCount64() + 3000;
			lastButtonIndex = buttonIndex;
			isActionComplete = true;
			return;
		}
	}
	

	//TODO: figure out combat ability, normal ability hotbuttons

	Buttons[buttonIndex] = "unrecognized button";
	return;
}
