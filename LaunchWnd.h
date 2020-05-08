#pragma once

class CElixirLaunchWnd : public CCustomWnd {
public:
	CComboWnd* CModeList;
	CButtonWnd* CButton;
	bool IsValid;
	bool IsLaunchEnabled;

	CElixirLaunchWnd(CXWnd* optionsWnd);
	~CElixirLaunchWnd();
	void DrawCombo();
	void LoadSettings();
	void SaveLoc();
	void LoadLoc(char szChar[64] = 0);
	int WndNotification(CXWnd* pWnd, unsigned int Message, void* unknown);
	void SaveSetting(PCHAR Key, PCHAR Value, ...);
private:
	CXWnd* optionsWnd;
};


