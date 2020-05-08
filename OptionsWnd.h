#pragma once
#include "../MQ2Plugin.h"

class CElixirOptionsWnd : public CCustomWnd {
public:
	CTabWnd* Tabs;
	CComboWnd* CDefaultStanceModeList;
	CCheckBoxWnd* CEnableLaunch;

	bool IsValid;

	CElixirOptionsWnd(CXWnd* launchWnd);
	~CElixirOptionsWnd();
	void DrawCombo();
	void LoadSettings(bool isLaunchEnabled, int stanceMode);
	void SaveLoc();
	void LoadLoc(char szChar[64] = 0);
	int WndNotification(CXWnd* pWnd, unsigned int Message, void* unknown);
	void SaveSetting(PCHAR Key, PCHAR Value, ...);
private:
	CXWnd* launchWnd;
};
