// MQ2Elixir.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup.

#include <mq/Plugin.h>
#include "MQ2Elixir.h"

PreSetup("MQ2Elixir");
PLUGIN_VERSION(0.1);

void DrawElixirSettingsPanel()
{
	if (!pElixir) {
		return;
	}
	ImGui::Checkbox("Buff AI", &pElixir->IsBuffAIRunning);

	ImGui::Checkbox("Heal AI", &pElixir->IsHealAIRunning);
	ImGui::Text("%% <=");
	ImGui::SameLine();
	ImGui::SliderInt("##elixir.healAIMax", &pElixir->HealAIMax, 10, 90);

	ImGui::Checkbox("Hurt Enemy AI", &pElixir->IsHateAIRunning);
	ImGui::Text("%% >=");
	ImGui::SameLine();
	ImGui::SliderInt("##elixir.hateAIMax", &pElixir->HateAIMax, 10, 90);
	

	ImGui::Separator();

	if (ImGui::Button("Reload Settings"))
	{
		LoadINI();
	}

	ImGui::SameLine();
	if (ImGui::Button("Reset Settings"))
	{
		//s_settings.Reset();
	}

}

/**
 * @fn InitializePlugin
 *
 * This is called once on plugin initialization and can be considered the startup
 * routine for the plugin.
 */
PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("MQ2Elixir::Initializing version %f", MQ2Version);

	pElixir = new Elixir();
	
	if (GetCharInfo()) {
		char szName[256] = { 0 };
		strcpy_s(szName, GetCharInfo()->Name);
		pElixir->StanceMode = GetPrivateProfileInt(szName, "Options-DefaultStanceMode", 0, INIFileName);
	}

	//Add commands, MQ2Data items, hooks, etc.
	AddCommand("/elixir", ElixirCommand);
	AddSettingsPanel("plugins/Elixir", DrawElixirSettingsPanel);
}

/**
 * @fn ShutdownPlugin
 *
 * This is called once when the plugin has been asked to shutdown.  The plugin has
 * not actually shut down until this completes.
 */
PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("MQ2Elixir::Shutting down");	
	//TODO: fix unloading
	delete pElixir;
    RemoveCommand("/elixir");
}

/**
 * @fn OnCleanUI
 *
 * This is called once just before the shutdown of the UI system and each time the
 * game requests that the UI be cleaned.  Most commonly this happens when a
 * /loadskin command is issued, but it also occurs when reaching the character
 * select screen and when first entering the game.
 *
 * One purpose of this function is to allow you to destroy any custom windows that
 * you have created and cleanup any UI items that need to be removed.
 */
PLUGIN_API void OnCleanUI()
{
	// DebugSpewAlways("MQ2Elixir::OnCleanUI()");
}

/**
 * @fn OnReloadUI
 *
 * This is called once just after the UI system is loaded. Most commonly this
 * happens when a /loadskin command is issued, but it also occurs when first
 * entering the game.
 *
 * One purpose of this function is to allow you to recreate any custom windows
 * that you have setup.
 */
PLUGIN_API void OnReloadUI()
{
	// DebugSpewAlways("MQ2Elixir::OnReloadUI()");
}

/**
 * @fn OnDrawHUD
 *
 * This is called each time the Heads Up Display (HUD) is drawn.  The HUD is
 * responsible for the net status and packet loss bar.
 *
 * Note that this is not called at all if the HUD is not shown (default F11 to
 * toggle).
 *
 * Because the net status is updated frequently, it is recommended to have a
 * timer or counter at the start of this call to limit the amount of times the
 * code in this section is executed.
 */
PLUGIN_API void OnDrawHUD()
{
/*
	static std::chrono::steady_clock::time_point DrawHUDTimer = std::chrono::steady_clock::now();
	// Run only after timer is up
	if (std::chrono::steady_clock::now() > DrawHUDTimer)
	{
		// Wait half a second before running again
		DrawHUDTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
		DebugSpewAlways("MQ2Elixir::OnDrawHUD()");
	}
*/
}

/**
 * @fn SetGameState
 *
 * This is called when the GameState changes.  It is also called once after the
 * plugin is initialized.
 *
 * For a list of known GameState values, see the constants that begin with
 * GAMESTATE_.  The most commonly used of these is GAMESTATE_INGAME.
 *
 * When zoning, this is called once after @ref OnBeginZone @ref OnRemoveSpawn
 * and @ref OnRemoveGroundItem are all done and then called once again after
 * @ref OnEndZone and @ref OnAddSpawn are done but prior to @ref OnAddGroundItem
 * and @ref OnZoned
 *
 * @param GameState int - The value of GameState at the time of the call
 */
PLUGIN_API void SetGameState(int GameState)
{
	if (GameState != GAMESTATE_INGAME) return;
	if (!strcmp(ServerCharacterINI, GetCharInfo()->Name)) return;
	sprintf(ServerCharacterINI, "%s_%s", EQADDR_SERVERNAME, GetCharInfo()->Name);
	_snprintf_s(INIFileName, 260, "%s\\MQ2Elixir.ini", gszINIPath);
	LoadINI();
}


/**
 * @fn OnPulse
 *
 * This is called each time MQ2 goes through its heartbeat (pulse) function.
 *
 * Because this happens very frequently, it is recommended to have a timer or
 * counter at the start of this call to limit the amount of times the code in
 * this section is executed.
 */
PLUGIN_API void OnPulse()
{
	static std::chrono::steady_clock::time_point PulseTimer = std::chrono::steady_clock::now();
	if (GetGameState() != GAMESTATE_INGAME) {
		return;
	}

	if (PulseTimer > std::chrono::steady_clock::now()) {
		return;
	}

	if (IsZoning) {
		return;
	}

	if (!IsElixirRunning) {
		return;
	}

	PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

	PCHARINFO pChar = GetCharInfo();

	if (!pChar) return;
	
	pElixir->OnPulse();
	DebugSpewAlways("MQ2Elixir::OnPulse()");
}

/**
 * @fn OnWriteChatColor
 *
 * This is called each time WriteChatColor is called (whether by MQ2Main or by any
 * plugin).  This can be considered the "when outputting text from MQ" callback.
 *
 * This ignores filters on display, so if they are needed either implement them in
 * this section or see @ref OnIncomingChat where filters are already handled.
 *
 * If CEverQuest::dsp_chat is not called, and events are required, they'll need to
 * be implemented here as well.  Otherwise, see @ref OnIncomingChat where that is
 * already handled.
 *
 * For a list of Color values, see the constants for USERCOLOR_.  The default is
 * USERCOLOR_DEFAULT.
 *
 * @param Line const char* - The line that was passed to WriteChatColor
 * @param Color int - The type of chat text this is to be sent as
 * @param Filter int - (default 0)
 */
PLUGIN_API void OnWriteChatColor(const char* Line, int Color, int Filter)
{
	// DebugSpewAlways("MQ2Elixir::OnWriteChatColor(%s, %d, %d)", Line, Color, Filter);
}

/**
 * @fn OnIncomingChat
 *
 * This is called each time a line of chat is shown.  It occurs after MQ filters
 * and chat events have been handled.  If you need to know when MQ2 has sent chat,
 * consider using @ref OnWriteChatColor instead.
 *
 * For a list of Color values, see the constants for USERCOLOR_. The default is
 * USERCOLOR_DEFAULT.
 *
 * @param Line const char* - The line of text that was shown
 * @param Color int - The type of chat text this was sent as
 *
 * @return bool - Whether to filter this chat from display
 */
PLUGIN_API bool OnIncomingChat(const char* Line, DWORD Color)
{
	// DebugSpewAlways("MQ2Elixir::OnIncomingChat(%s, %d)", Line, Color);
	return false;
}

/**
 * @fn OnAddSpawn
 *
 * This is called each time a spawn is added to a zone (ie, something spawns). It is
 * also called for each existing spawn when a plugin first initializes.
 *
 * When zoning, this is called for all spawns in the zone after @ref OnEndZone is
 * called and before @ref OnZoned is called.
 *
 * @param pNewSpawn PSPAWNINFO - The spawn that was added
 */
PLUGIN_API void OnAddSpawn(PSPAWNINFO pNewSpawn)
{
	// DebugSpewAlways("MQ2Elixir::OnAddSpawn(%s)", pNewSpawn->Name);
}

/**
 * @fn OnRemoveSpawn
 *
 * This is called each time a spawn is removed from a zone (ie, something despawns
 * or is killed).  It is NOT called when a plugin shuts down.
 *
 * When zoning, this is called for all spawns in the zone after @ref OnBeginZone is
 * called.
 *
 * @param pSpawn PSPAWNINFO - The spawn that was removed
 */
PLUGIN_API void OnRemoveSpawn(PSPAWNINFO pSpawn)
{
	// DebugSpewAlways("MQ2Elixir::OnRemoveSpawn(%s)", pSpawn->Name);
}

/**
 * @fn OnAddGroundItem
 *
 * This is called each time a ground item is added to a zone (ie, something spawns).
 * It is also called for each existing ground item when a plugin first initializes.
 *
 * When zoning, this is called for all ground items in the zone after @ref OnEndZone
 * is called and before @ref OnZoned is called.
 *
 * @param pNewGroundItem PGROUNDITEM - The ground item that was added
 */
PLUGIN_API void OnAddGroundItem(PGROUNDITEM pNewGroundItem)
{
	// DebugSpewAlways("MQ2Elixir::OnAddGroundItem(%d)", pNewGroundItem->DropID);
}

/**
 * @fn OnRemoveGroundItem
 *
 * This is called each time a ground item is removed from a zone (ie, something
 * despawns or is picked up).  It is NOT called when a plugin shuts down.
 *
 * When zoning, this is called for all ground items in the zone after
 * @ref OnBeginZone is called.
 *
 * @param pGroundItem PGROUNDITEM - The ground item that was removed
 */
PLUGIN_API void OnRemoveGroundItem(PGROUNDITEM pGroundItem)
{
	// DebugSpewAlways("MQ2Elixir::OnRemoveGroundItem(%d)", pGroundItem->DropID);
}

/**
 * @fn OnBeginZone
 *
 * This is called just after entering a zone line and as the loading screen appears.
 */
PLUGIN_API void OnBeginZone()
{
	IsZoning = true;
}

/**
 * @fn OnEndZone
 *
 * This is called just after the loading screen, but prior to the zone being fully
 * loaded.
 *
 * This should occur before @ref OnAddSpawn and @ref OnAddGroundItem are called. It
 * always occurs before @ref OnZoned is called.
 */
PLUGIN_API void OnEndZone()
{
	IsZoning = false;
}

/**
 * @fn OnZoned
 *
 * This is called after entering a new zone and the zone is considered "loaded."
 *
 * It occurs after @ref OnEndZone @ref OnAddSpawn and @ref OnAddGroundItem have
 * been called.
 */
PLUGIN_API void OnZoned()
{
	// DebugSpewAlways("MQ2Elixir::OnZoned()");
	pElixir->ZoneCooldownTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(5000);
}

/**
 * @fn OnUpdateImGui
 *
 * This is called each time that the ImGui Overlay is rendered. Use this to render
 * and update plugin specific widgets.
 *
 * Because this happens extremely frequently, it is recommended to move any actual
 * work to a separate call and use this only for updating the display.
 */
PLUGIN_API void OnUpdateImGui()
{
	/*
	bool ShowMQ2ElixirWindow = true;
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (ShowMQ2ElixirWindow)
		{
			if (ImGui::Begin("MQ2Elixir", &ShowMQ2ElixirWindow, ImGuiWindowFlags_MenuBar))
			{
				if (ImGui::BeginMenuBar())
				{
					ImGui::Text("MQ2Elixir is loaded!");
					ImGui::EndMenuBar();
				}
			}
			ImGui::End();
		}
	}
	*/
}

/**
 * @fn OnMacroStart
 *
 * This is called each time a macro starts (ex: /mac somemacro.mac), prior to
 * launching the macro.
 *
 * @param Name const char* - The name of the macro that was launched
 */
PLUGIN_API void OnMacroStart(const char* Name)
{
	// DebugSpewAlways("MQ2Elixir::OnMacroStart(%s)", Name);
}

/**
 * @fn OnMacroStop
 *
 * This is called each time a macro stops (ex: /endmac), after the macro has ended.
 *
 * @param Name const char* - The name of the macro that was stopped.
 */
PLUGIN_API void OnMacroStop(const char* Name)
{
	// DebugSpewAlways("MQ2Elixir::OnMacroStop(%s)", Name);
}

/**
 * @fn OnLoadPlugin
 *
 * This is called each time a plugin is loaded (ex: /plugin someplugin), after the
 * plugin has been loaded and any associated -AutoExec.cfg file has been launched.
 * This means it will be executed after the plugin's @ref InitializePlugin callback.
 *
 * This is also called when THIS plugin is loaded, but initialization tasks should
 * still be done in @ref InitializePlugin.
 *
 * @param Name const char* - The name of the plugin that was loaded
 */
PLUGIN_API void OnLoadPlugin(const char* Name)
{
	// DebugSpewAlways("MQ2Elixir::OnLoadPlugin(%s)", Name);
}

/**
 * @fn OnUnloadPlugin
 *
 * This is called each time a plugin is unloaded (ex: /plugin someplugin unload),
 * just prior to the plugin unloading.  This means it will be executed prior to that
 * plugin's @ref ShutdownPlugin callback.
 *
 * This is also called when THIS plugin is unloaded, but shutdown tasks should still
 * be done in @ref ShutdownPlugin.
 *
 * @param Name const char* - The name of the plugin that is to be unloaded
 */
PLUGIN_API void OnUnloadPlugin(const char* Name)
{
	// DebugSpewAlways("MQ2Elixir::OnUnloadPlugin(%s)", Name);
}

void UpdateINIFileName()
{
	sprintf_s(INIFileName, "%s\\%s_%s.ini", gPathConfig, GetServerShortName(), pLocalPC->Name);
}

void SaveINI() {
	UpdateINIFileName();
	std::string sectionName = "Elixir";
	WritePrivateProfileInt(sectionName, "HealAIMax", pElixir->HealAIMax, INIFileName);
	WritePrivateProfileBool(sectionName, "HealAIEnabled", pElixir->IsHealAIRunning, INIFileName);
	WritePrivateProfileInt(sectionName, "HateAIMax", pElixir->HateAIMax, INIFileName);
	WritePrivateProfileBool(sectionName, "HateAIEnabled", pElixir->IsHateAIRunning, INIFileName);
	WritePrivateProfileBool(sectionName, "BuffAIEnabled", pElixir->IsBuffAIRunning, INIFileName);
	WritePrivateProfileBool(sectionName, "AIEnabled", IsElixirRunning, INIFileName);
}

void LoadINI() {
	UpdateINIFileName();
	std::string sectionName = "Elixir";
	IsElixirRunning = GetPrivateProfileInt(sectionName, "AIEnabled", 1, INIFileName);
	pElixir->HealAIMax = GetPrivateProfileInt(sectionName, "HealAIMax", 50, INIFileName);
	pElixir->IsHealAIRunning = GetPrivateProfileInt(sectionName, "HealAIEnabled", 1, INIFileName);
	pElixir->HateAIMax = GetPrivateProfileInt(sectionName, "HateAIMax", 80, INIFileName);
	pElixir->IsHateAIRunning = GetPrivateProfileInt(sectionName, "HateAIEnabled", 0, INIFileName);
	pElixir->IsBuffAIRunning = GetPrivateProfileInt(sectionName, "BuffAIEnabled", 0, INIFileName);
}

PLUGIN_API void ElixirCommand(PSPAWNINFO pLPlayer, char* szLine)
{
	if (!GetPcProfile()) return;
	std::string sectionName = "Elixir";
	char szCommand[MAX_STRING] = { 0 };
	char szArg[MAX_STRING] = { 0 };
	char szArg2[MAX_STRING] = { 0 };
	GetArg(szCommand, szLine, 1);
	GetArg(szArg, szLine, 2);
	GetArg(szArg2, szLine, 3);
	if (!*szCommand || !_strnicmp(szCommand, "help", 5)) {
		WriteChatf("usage: /elixir <command> [subCommands] (current value): description");
		WriteChatf("elixir <ai> [0|1] (\a%s\ax): overall AI running and management", (IsElixirRunning ? "g1" : "r0"));
		WriteChatf("elixir <hate> [ai:(\a%s\ax)|max:(\ag%d\ax)]: set smart hate AI values", (pElixir->IsHateAIRunning ? "g1" : "r0"), pElixir->HateAIMax);
		WriteChatf("elixir <buff> [ai:(\a%s\ax)]: set smart buff AI values", (pElixir->IsBuffAIRunning ? "g1" : "r0"));
		WriteChatf("elixir <heal> [ai:(\a%s\ax)|max:(\ag%d\ax)]: set smart heal AI values", (pElixir->IsHealAIRunning ? "g1" : "r0"), pElixir->HealAIMax);
		WriteChatf("elixir <loadini>: reload ini values");
		WriteChatf("elixir <saveini>: save current settings to ini");
		WriteChatf("elixir <debug> [tagmode:(\a%s\ax)]: enable debug options", (pElixir->IsDebugTagMode ? "g1" : "r0"));
		return;
	}
	if (!_strnicmp(szCommand, "enable", 7)) {
		IsElixirRunning = true;
		return;
	}

	if (!_strnicmp(szCommand, "disable", 8)) {
		IsElixirRunning = true;
		return;
	}

	if (!_strnicmp(szCommand, "loadini", 8)) {
		LoadINI();
		WriteChatf("elixir ini reloaded");
		return;
	}

	if (!_strnicmp(szCommand, "saveini", 8)) {		
		SaveINI();
		WriteChatf("elixir ini saved");
		return;
	}

	if (!_strnicmp(szCommand, "hate", 4)) {
		if (!_strnicmp(szArg, "ai", 3)) {
			if (!_strnicmp(szArg2, "1", 2)) {
				pElixir->IsHateAIRunning = true;
				WriteChatf("elixir hate ai is now enabled");
				WritePrivateProfileInt(sectionName, "IsHateAIEnabled", 1, INIFileName);
				return;
			}
			if (!_strnicmp(szArg2, "0", 2)) {
				pElixir->IsHateAIRunning = false;
				WriteChatf("elixir hate ai is now disabled");
				WritePrivateProfileInt(sectionName, "IsHateAIEnabled", 0, INIFileName);
				return;
			}
			WriteChatf("elixir hate ai [0|1] (\a%s\ax): set smart hate AI values", (pElixir->IsHateAIRunning ? "g1" : "r0"));
			return;
		}
		if (!_strnicmp(szArg, "max", 4)) {
			if (!*szArg2) {
				WriteChatf("/elixir hate max (\ag%d\ax): set smart hate AI threshold", pElixir->HateAIMax);
				return;
			}
			if (!IsNumber(szArg2)) {
				WriteChatf("invalid number: %s", szArg2);
				return;
			}
			if (atoi(szArg2) < 1) {
				WriteChatf("invalid number: %s, must be greater than 0", szArg2);
			}
			if (atoi(szArg2) > 100) {
				WriteChatf("invalid number: %s, must be less than 100", szArg2);
			}

			pElixir->HateAIMax = atoi(szArg2);

			WriteChatf("elixir hate max threshold is set to %d", pElixir->HateAIMax);
			return;
		}

		WriteChatf("elixir <hate> [ai:(\a%s\ax)|max:(\ag%d\ax)]: set smart hate AI values", (pElixir->IsHateAIRunning ? "g1" : "r0"), pElixir->HateAIMax);
		return;
	}

	if (!_strnicmp(szCommand, "heal", 4)) {
		if (!_strnicmp(szArg, "ai", 3)) {
			if (!_strnicmp(szArg2, "1", 2)) {
				pElixir->IsHealAIRunning = true;
				WriteChatf("elixir heal ai is now enabled");
				return;
			}
			if (!_strnicmp(szArg2, "0", 2)) {
				pElixir->IsHealAIRunning = false;
				WriteChatf("elixir heal ai is now disabled");
				return;
			}
			WriteChatf("elixir heal ai [0|1] (\a%s\ax): set smart heal AI values", (pElixir->IsHealAIRunning ? "g1" : "r0"));
			return;
		}
		if (!_strnicmp(szArg, "max", 4)) {
			if (!*szArg2) {
				WriteChatf("elixir heal max (\ag%d\ax): set smart hate AI threshold", pElixir->HealAIMax);
				return;
			}
			if (!IsNumber(szArg2)) {
				WriteChatf("invalid number: %s", szArg2);
				return;
			}
			if (atoi(szArg2) < 1) {
				WriteChatf("invalid number: %s, must be greater than 0", szArg2);
			}
			if (atoi(szArg2) > 100) {
				WriteChatf("invalid number: %s, must be less than 100", szArg2);
			}

			pElixir->HealAIMax = atoi(szArg2);
			WriteChatf("elixir heal max threshold is set to %d", pElixir->HealAIMax);
			return;
		}

		WriteChatf("elixir <heal> [ai|max] (\a%s\ax): set smart heal AI threshold", (pElixir->IsHealAIRunning ? "g1" : "r0"));
		return;
	}


	if (!_strnicmp(szCommand, "buff", 4)) {
		if (!_strnicmp(szArg, "ai", 3)) {
			if (!_strnicmp(szArg2, "1", 2)) {
				pElixir->IsBuffAIRunning = true;
				WriteChatf("elixir buff ai is now enabled");
				return;
			}
			if (!_strnicmp(szArg2, "0", 2)) {
				pElixir->IsBuffAIRunning = false;
				WriteChatf("elixir buff ai is now disabled");
				return;
			}
			WriteChatf("elixir buff ai [0|1] (\a%s\ax): set smart buff AI values", (pElixir->IsBuffAIRunning ? "g1" : "r0"));
			return;
		}

		WriteChatf("elixir <buff> [ai] (\a%s\ax): set smart buff AI threshold", (pElixir->IsBuffAIRunning ? "g1" : "r0"));
		return;
	}


	if (!_strnicmp(szCommand, "debug", 6)) {
		if (!_strnicmp(szArg, "tagmode", 8)) {
			if (!_strnicmp(szArg2, "1", 2)) {
				pElixir->IsDebugTagMode = true;
				WriteChatf("elixir debug tagmode is now enabled");
				return;
			}
			if (!_strnicmp(szArg2, "0", 2)) {
				pElixir->IsDebugTagMode = false;
				WriteChatf("elixir debug tagmode is now disabled");
				return;
			}
			WriteChatf("elixir debug [tagmode:(\a%s\ax)]: enable debug options", (pElixir->IsDebugTagMode ? "g1" : "r0"));
			return;
		}

		WriteChatf("elixir <debug> [tagmode:(\a%s\ax)]: enable debug options", (pElixir->IsDebugTagMode ? "g1" : "r0"));
		return;
	}
}