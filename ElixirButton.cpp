#include "../MQ2Plugin.h"
#include "Core.h"
#include "Elixir.h"

using namespace std;

void Elixir::AttemptButton(int buttonIndex)
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) {
		buttons[buttonIndex] = "char not loaded";
		return;
	}

	if (ZoneCooldown > MQGetTickCount64()) {
		buttons[buttonIndex] = "zone cooldown";
		return;
	}

	if (pChar->pSpawn->GetClass() != Bard && isActionComplete) {
		buttons[buttonIndex] = "earlier action complete";
		return;
	}

	if (GetCharInfo()->Stunned) {
		buttons[buttonIndex] = "player stunned";
		return;
	}

	if (GetCharInfo()->pSpawn->HideMode) {
		buttons[buttonIndex] = "player invisible";
	}

	if (lastButtonIndex == buttonIndex && lastActionRepeatCooldown > MQGetTickCount64()) {
		buttons[buttonIndex] = "last action repeat cooldown";
		return;
	}

	CXWnd* wnd = FindMQ2Window("HotButtonWnd");
	if (!wnd) {
		buttons[buttonIndex] = "button window not found";
		return;
	}

	CXWnd* child = wnd->GetChildItem("HB_Button" + buttonIndex + 1);
	if (!child) {
		buttons[buttonIndex] = "button area not found";
		return;
	}

	CHotButton* hbtn = (CHotButton*)child;
	if (!hbtn) {
		buttons[buttonIndex] = "button not set";
	}


	CXWnd* child2 = child->GetChildItem("HB_SpellGem");
	if (child2) {
		//TODO: iterate gems, figure out which one is this hotbutton, and AttemptGem(gemIndex);
		if (isActionComplete) {
			LastAction = "pressing hotbutton " + buttonIndex;
			buttons[buttonIndex] = LastAction;
			lastActionRepeatCooldown = (unsigned long)MQGetTickCount64() + 3000;
			lastButtonIndex = buttonIndex;
			isActionComplete = true;
			return;
		}
	}

	child2 = child->GetChildItem("HB_InvSlot");
	//TODO: attempt to click item
	if (isActionComplete) {
		LastAction = "pressing hotbutton " + buttonIndex;
		buttons[buttonIndex] = LastAction;
		lastActionRepeatCooldown = (unsigned long)MQGetTickCount64() + 3000;
		lastButtonIndex = buttonIndex;
		isActionComplete = true;
		return;
	}

	//TODO: figure out combat ability, normal ability hotbuttons

	buttons[buttonIndex] = "unrecognized button";
	return;
}
