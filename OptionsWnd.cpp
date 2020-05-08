#include "OptionsWnd.h"
#include "../MQ2Plugin.h"

CElixirOptionsWnd::CElixirOptionsWnd(CXWnd* launchWnd) :CCustomWnd("ElixirOptionsWnd") {
	bool isUIValid = true;
	this->launchWnd = launchWnd;
	if (!(Tabs = (CTabWnd*)GetChildItem("Elixir_Tabs"))) isUIValid = false;
	if (!(CDefaultStanceModeList = (CComboWnd*)GetChildItem("Elixir_DefaultMode"))) isUIValid = false;
	if (!(CEnableLaunch = (CCheckBoxWnd*)GetChildItem("Elixir_EnableLaunch"))) isUIValid = false;

	this->SetBGColor(0xFF000000);//Setting this here sets it for the entire window. Setting everything individually was blacking out the checkboxes!

	if (!isUIValid) {
		WriteChatf("\ar[MQ2Elixir] Incorrect UI File in use. Please update to latest and reload plugin.");
		IsValid = false; 
		return;
	}
	IsValid = true;
	SetWndNotification(CElixirOptionsWnd);
	Tabs->UpdatePage();
	DrawCombo();
}
CElixirOptionsWnd::~CElixirOptionsWnd() {}



void CElixirOptionsWnd::DrawCombo() {
	int selection = CDefaultStanceModeList->GetCurChoice();
	CDefaultStanceModeList->DeleteAll();

	CDefaultStanceModeList->InsertChoice("Elixir");
	CDefaultStanceModeList->InsertChoice("Clutch");
	CDefaultStanceModeList->InsertChoice("Minor");
	CDefaultStanceModeList->InsertChoice("None");

	CDefaultStanceModeList->SetChoice(selection);
}


void CElixirOptionsWnd::SaveLoc() {
	if (!GetCharInfo()) return;
	CHAR szTemp[MAX_STRING] = { 0 };
	WritePrivateProfileString(GetCharInfo()->Name, "Options-Saved", "1", INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().top);
	WritePrivateProfileString(GetCharInfo()->Name, "Options-Top", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().bottom);
	WritePrivateProfileString(GetCharInfo()->Name, "Options-Bottom", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().left);
	WritePrivateProfileString(GetCharInfo()->Name, "Options-Left", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetLocation().right);
	WritePrivateProfileString(GetCharInfo()->Name, "Options-Right", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetAlpha());
	WritePrivateProfileString(GetCharInfo()->Name, "Options-Alpha", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", GetFadeToAlpha());
	WritePrivateProfileString(GetCharInfo()->Name, "Options-FadeToAlpha", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", CDefaultStanceModeList->GetCurChoice());
	WritePrivateProfileString(GetCharInfo()->Name, "Options-DefaultStanceMode", szTemp, INIFileName);
	sprintf_s(szTemp, "%i", CEnableLaunch->Checked ? 1 : 0);
	WritePrivateProfileString(GetCharInfo()->Name, "Options-EnableLaunch", szTemp, INIFileName);
}


void CElixirOptionsWnd::LoadSettings(bool isLaunchEnabled, int defaultStanceMode) {
	CEnableLaunch->Checked = isLaunchEnabled;
	DrawCombo();
	CDefaultStanceModeList->SetChoice(defaultStanceMode);
}

void CElixirOptionsWnd::LoadLoc(char szChar[256]) {
	if (!GetCharInfo()) return;
	if (!this->IsValid) return;

	char szName[256] = { 0 };
	if (!szChar) strcpy_s(szName, GetCharInfo()->Name);
	else strcpy_s(szName, szChar);

	bool isSaved = (GetPrivateProfileInt(szName, "Options-Saved", 0, INIFileName) > 0 ? true : false);
	if (isSaved) {
		SetLocation({
			(LONG)GetPrivateProfileInt(szName, "Options-Left", 0, INIFileName),
			(LONG)GetPrivateProfileInt(szName, "Options-Top", 0, INIFileName),
			(LONG)GetPrivateProfileInt(szName, "Options-Right", 0, INIFileName),
			(LONG)GetPrivateProfileInt(szName, "Options-Bottom", 0, INIFileName)
			});

		SetAlpha((BYTE)GetPrivateProfileInt(szName, "Options-Alpha", 0, INIFileName));
		SetFadeToAlpha((BYTE)GetPrivateProfileInt(szName, "Options-FadeToAlpha", 0, INIFileName));
	}
	
	int defaultStanceMode = GetPrivateProfileInt(szName, "Options-DefaultStanceMode", 0, INIFileName);
	bool isLaunchEnabled = (GetPrivateProfileInt(szName, "Options-EnableLaunch", 0, INIFileName) > 0 ? true : false);
	LoadSettings(isLaunchEnabled, defaultStanceMode);
}

int CElixirOptionsWnd::WndNotification(CXWnd* pWnd, unsigned int Message, void* unknown) {
	int retVal = CSidlScreenWnd::WndNotification(pWnd, Message, unknown);
	if (!GetCharInfo()) return retVal;
	CHAR szTemp[MAX_STRING] = { 0 };

	if (Message != XWM_LCLICK) return retVal;
	
	//if (pWnd == (CXWnd*)Tabs) LoadSettings();
	if (pWnd == (CXWnd*)CEnableLaunch) {
		sprintf_s(szTemp, "%i", CEnableLaunch->Checked ? 1 : 0);
		WritePrivateProfileString(GetCharInfo()->Name, "Options-EnableLaunch", szTemp, INIFileName);

		if (!launchWnd) return retVal;
		if (launchWnd->IsVisible() != CEnableLaunch->Checked) launchWnd->SetVisible(CEnableLaunch->Checked);
		return retVal;
	}

	if (pWnd == (CXWnd*)CDefaultStanceModeList) {
		sprintf_s(szTemp, "%i", CDefaultStanceModeList->GetCurChoice());
		WritePrivateProfileString(GetCharInfo()->Name, "Options-DefaultStanceMode", szTemp, INIFileName);
		return retVal;
	}

	return retVal;

};
