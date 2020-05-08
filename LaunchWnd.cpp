#include "../MQ2Plugin.h"
#include "LaunchWnd.h"

CElixirLaunchWnd::CElixirLaunchWnd(CXWnd* optionsWnd) :CCustomWnd("ElixirLaunchWnd") {
	bool isUIValid = true;

	if (!(CModeList = (CComboWnd*)GetChildItem("Elixir_Mode"))) isUIValid = false;
	if (!(CButton = (CButtonWnd*)GetChildItem("Elixir_Button"))) isUIValid = false;	
	//this->SetBGColor(0xFF000000);//Setting this here sets it for the entire window. Setting everything individually was blacking out the checkboxes!

	/*if (pCharSpawn) {
		switch (pCharSpawn->GetClass()) {
		case Shaman:
			//CXWndDrawTemplate* drawTmp = new CXWndDrawTemplate();
			//CButton->SetDrawTemplate(drawTmp)
		}
	}*/

	if (!isUIValid) {
		WriteChatf("\ar[MQ2Elixir] Incorrect UI File in use. Please update to latest and reload plugin.");
		IsValid = false;
		return;
	}
	IsValid = true;
	this->optionsWnd = optionsWnd;


	LoadLoc();
	SetWndNotification(CElixirLaunchWnd);
	DrawCombo();
}
CElixirLaunchWnd::~CElixirLaunchWnd() {}


void CElixirLaunchWnd::DrawCombo() {
	int selection = CModeList->GetCurChoice();
	CModeList->DeleteAll();

	CModeList->InsertChoice("Elixir");
	CModeList->InsertChoice("Clutch");
	CModeList->InsertChoice("Minor");
	CModeList->InsertChoice("None");

	CModeList->SetChoice(selection);
}

void CElixirLaunchWnd::SaveLoc() {
	if (!GetCharInfo()) return;
	CHAR szTemp[MAX_STRING] = { 0 };
	WritePrivateProfileString(GetCharInfo()->Name, "Saved", "1", INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().top);
	WritePrivateProfileString(GetCharInfo()->Name, "Top", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().bottom);
	WritePrivateProfileString(GetCharInfo()->Name, "Bottom", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().left);
	WritePrivateProfileString(GetCharInfo()->Name, "Left", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().right);
	WritePrivateProfileString(GetCharInfo()->Name, "Right", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetAlpha());
	WritePrivateProfileString(GetCharInfo()->Name, "Alpha", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetFadeToAlpha());
	WritePrivateProfileString(GetCharInfo()->Name, "FadeToAlpha", szTemp, INIFileName);
}


void CElixirLaunchWnd::LoadSettings() {
	DrawCombo();
}


void CElixirLaunchWnd::LoadLoc(char szChar[256]) {
	if (!GetCharInfo()) return;
	if (!this->IsValid) return;

	char szName[256] = { 0 };
	if (!szChar) strcpy_s(szName, GetCharInfo()->Name);
	else strcpy_s(szName, szChar);

	bool isSaved = (GetPrivateProfileInt(szName, "Saved", 0, INIFileName) > 0 ? true : false);
	if (isSaved) {
		SetLocation({
			(LONG)GetPrivateProfileInt(szName, "Left", 0, INIFileName),
			(LONG)GetPrivateProfileInt(szName, "Top", 0, INIFileName),
			(LONG)GetPrivateProfileInt(szName, "Right", 0, INIFileName),
			(LONG)GetPrivateProfileInt(szName, "Bottom", 0, INIFileName)
			});

		SetAlpha((BYTE)GetPrivateProfileInt(szName, "Alpha", 0, INIFileName));
		SetFadeToAlpha((BYTE)GetPrivateProfileInt(szName, "FadeToAlpha", 0, INIFileName));
	}
	isSaved = (GetPrivateProfileInt(szName, "Options-Saved", 0, INIFileName) > 0 ? true : false);
	LoadSettings();
}


int CElixirLaunchWnd::WndNotification(CXWnd* pWnd, unsigned int Message, void* unknown) {
	//if (Message == XWM_CLOSE) CheckActive();
	int retVal = CSidlScreenWnd::WndNotification(pWnd, Message, unknown);

	if (Message != XWM_LCLICK) return retVal;
	if (pWnd == (CXWnd*)CButton) {
		if (!pOptionsWnd) return retVal;
		if (pOptionsWnd->IsVisible()) return retVal;
		pOptionsWnd->SetVisible(true);
		return retVal;
	}

	if (pWnd == (CXWnd*)CModeList) {

		return retVal;
	}

	return retVal;
};
