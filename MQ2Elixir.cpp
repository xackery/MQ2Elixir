// MQ2Elixir.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup.

#include <mq/Plugin.h>
#include "MQ2Elixir.h"
#include <mq/imgui/ImGuiUtils.h>

PreSetup("MQ2Elixir");
PLUGIN_VERSION(0.1);

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

	//Add commands, MQ2Data items, hooks, etc.
	AddCommand("/elixir", ElixirCommand);
	AddSettingsPanel("plugins/Elixir", DrawElixirUIPanel);
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
	RemoveSettingsPanel("plugins/Elixir");
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
	if (!pLocalPC) return;
	if (!strcmp(ServerCharacterINI, pLocalPC->Name)) return;
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

	if (!pLocalPC) {
		return;
	}

	if (PulseTimer > std::chrono::steady_clock::now()) {
		return;
	}

	if (IsZoning) {
		return;
	}

	if (!pElixir) {
		return;
	}
	if (!pElixir->IsElixirRunning) {
		return;
	}

	PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

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
	pElixir->ClearCharmTarget();
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
	if (!pElixir) return;
	if (!pElixir->IsElixirUIShowing) return;

	if (GetGameState() != GAMESTATE_INGAME) return;	

	if (ImGui::Begin(pElixir->ElixirPluginVersion, &pElixir->IsElixirUIShowing))//, ImGuiWindowFlags_MenuBar))
	{
		DrawElixirUI();
	}
	ImGui::End();
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
	WritePrivateProfileBool(sectionName, "AIEnabled", pElixir->IsElixirRunning, INIFileName);
	WritePrivateProfileBool(sectionName, "DisableOnFocus", pElixir->IsElixirDisabledOnFocus, INIFileName);
	
	WritePrivateProfileBool(sectionName, "HateAIEnabled", pElixir->IsHateAIRunning, INIFileName);
	WritePrivateProfileInt(sectionName, "HateAIMax", pElixir->HateAIMax, INIFileName);
	
	WritePrivateProfileBool(sectionName, "HealAIEnabled", pElixir->IsHealAIRunning, INIFileName);
	WritePrivateProfileInt(sectionName, "HealAIMax", pElixir->HealAIMax, INIFileName);

	WritePrivateProfileBool(sectionName, "BuffAIEnabled", pElixir->IsBuffAIRunning, INIFileName);
	WritePrivateProfileBool(sectionName, "BuffAIDuringCombat", pElixir->IsBuffDuringCombat, INIFileName);
	
	WritePrivateProfileBool(sectionName, "NukeAIEnabled", pElixir->IsNukeAIRunning, INIFileName);
	WritePrivateProfileInt(sectionName, "NukeAIManaMax", pElixir->NukeAIManaMax, INIFileName);
	WritePrivateProfileInt(sectionName, "NukeAIHPMax", pElixir->NukeAIHPMax, INIFileName);

	WritePrivateProfileBool(sectionName, "MeditateAIEnabled", pElixir->IsMeditateAIRunning, INIFileName);
	WritePrivateProfileBool(sectionName, "MeditateBySittingEnabled", pElixir->IsMeditateBySitting, INIFileName);
	WritePrivateProfileBool(sectionName, "MeditateDuringCombatEnabled", pElixir->IsMeditateEnabledDuringCombat, INIFileName);

	WritePrivateProfileBool(sectionName, "TargetAIEnabled", pElixir->IsTargetAIRunning, INIFileName);	
	WritePrivateProfileInt(sectionName, "TargetAIMinRange", pElixir->TargetAIMinRange, INIFileName);
	WritePrivateProfileBool(sectionName, "TargetAIAutoAttack", pElixir->IsTargetAutoAttack, INIFileName);
	WritePrivateProfileBool(sectionName, "TargetAIPetAttack", pElixir->IsTargetPetAttack, INIFileName);

	WritePrivateProfileBool(sectionName, "CharmAIEnabled", pElixir->IsCharmAIRunning, INIFileName);

	WritePrivateProfileBool(sectionName, "SettingsDebugEnabled", pElixir->IsSettingsDebugEnabled, INIFileName);
	char szBuffer[MAX_STRING] = { 0 };

	for (int i = 0; i < NUM_SPELL_GEMS; i++) {
		std::string ignoreGemStr = "IgnoreGem";
		ignoreGemStr += std::to_string((int)(i + 1));
		WritePrivateProfileBool(sectionName, ignoreGemStr, pElixir->IgnoreGems[i], INIFileName);
	}
}

void LoadINI() {
	UpdateINIFileName();
	DebugSpewAlways("MQElixir::LoadINI");
	std::string sectionName = "Elixir";
	pElixir->IsElixirRunning = GetPrivateProfileBool(sectionName, "AIEnabled", true, INIFileName);
	pElixir->IsElixirDisabledOnFocus = GetPrivateProfileBool(sectionName, "DisableOnFocus", false, INIFileName);

	pElixir->IsHateAIRunning = GetPrivateProfileBool(sectionName, "HateAIEnabled", false, INIFileName);
	pElixir->HateAIMax = GetPrivateProfileInt(sectionName, "HateAIMax", 80, INIFileName);
		
	pElixir->IsHealAIRunning = GetPrivateProfileBool(sectionName, "HealAIEnabled", true, INIFileName);
	pElixir->HealAIMax = GetPrivateProfileInt(sectionName, "HealAIMax", 50, INIFileName);

	pElixir->IsBuffAIRunning = GetPrivateProfileBool(sectionName, "BuffAIEnabled", false, INIFileName);
	pElixir->IsBuffDuringCombat = GetPrivateProfileBool(sectionName, "BuffAIDuringCombat", false, INIFileName);

	pElixir->IsNukeAIRunning = GetPrivateProfileBool(sectionName, "NukeAIEnabled", false, INIFileName);
	pElixir->NukeAIHPMax = GetPrivateProfileInt(sectionName, "NukeAIHPMax", 95, INIFileName);
	pElixir->NukeAIManaMax = GetPrivateProfileInt(sectionName, "NukeAIManaMax", 80, INIFileName);

	pElixir->IsMeditateAIRunning = GetPrivateProfileBool(sectionName, "MeditateAIEnabled", false, INIFileName);
	pElixir->IsMeditateBySitting = GetPrivateProfileBool(sectionName, "MeditateBySittingEnabled", false, INIFileName);
	pElixir->IsMeditateEnabledDuringCombat = GetPrivateProfileBool(sectionName, "MeditateDuringCombatEnabled", true, INIFileName);
	
	pElixir->IsTargetAIRunning = GetPrivateProfileBool(sectionName, "TargetAIEnabled", false, INIFileName);	
	pElixir->TargetAIMinRange = GetPrivateProfileInt(sectionName, "TargetAIMinRange", 80, INIFileName);
	pElixir->IsTargetAutoAttack= GetPrivateProfileBool(sectionName, "TargetAIAutoAttack", false, INIFileName);
	pElixir->IsTargetPetAttack = GetPrivateProfileBool(sectionName, "TargetAIPetAttack", false, INIFileName);

	pElixir->IsCharmAIRunning = GetPrivateProfileBool(sectionName, "CharmAIEnabled", false, INIFileName);

	pElixir->IsSettingsDebugEnabled = GetPrivateProfileInt(sectionName, "SettingsDebugEnabled", 80, INIFileName);
	
	for (int i = 0; i < NUM_SPELL_GEMS; i++) {
		std::string ignoreGemStr = "IgnoreGem";
		ignoreGemStr += std::to_string((int)(i + 1));
		pElixir->IgnoreGems[i] = GetPrivateProfileBool(sectionName, ignoreGemStr, 0, INIFileName);
	}
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
		WriteChatf("elixir <ai> [0|1] (\a%s\ax): overall AI running and management", (pElixir->IsElixirRunning ? "g1" : "r0"));
		WriteChatf("elixir <ui>: show elixir UI");
		WriteChatf("elixir <target> [ai:(\a%s\ax)]: set auto target AI values", (pElixir->IsTargetAIRunning ? "g1" : "r0"));
		WriteChatf("elixir <hate> [ai:(\a%s\ax)|max:(\ag%d\ax)]: set smart hate AI values", (pElixir->IsHateAIRunning ? "g1" : "r0"), pElixir->HateAIMax);
		WriteChatf("elixir <buff> [ai:(\a%s\ax)]: set smart buff AI values", (pElixir->IsBuffAIRunning ? "g1" : "r0"));
		WriteChatf("elixir <heal> [ai:(\a%s\ax)|max:(\ag%d\ax)]: set smart heal AI values", (pElixir->IsHealAIRunning ? "g1" : "r0"), pElixir->HealAIMax);
		WriteChatf("elixir <loadini>: reload ini values");
		WriteChatf("elixir <saveini>: save current settings to ini");
		WriteChatf("elixir <debug> [tagmode:(\a%s\ax)]: enable debug options", (pElixir->IsDebugTagMode ? "g1" : "r0"));
		return;
	}
	if (!_strnicmp(szCommand, "enable", 7)) {
		pElixir->IsElixirRunning = true;
		return;
	}

	if (!_strnicmp(szCommand, "disable", 8)) {
		pElixir->IsElixirRunning = true;
		return;
	}

	if (!_strnicmp(szCommand, "ui", 3)) {
		pElixir->IsElixirUIShowing = true;
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

	
	if (!_strnicmp(szCommand, "target", 4)) {
		if (!_strnicmp(szArg, "ai", 3)) {
			if (!_strnicmp(szArg2, "1", 2)) {
				pElixir->IsBuffAIRunning = true;
				WriteChatf("elixir target ai is now enabled");
				return;
			}
			if (!_strnicmp(szArg2, "0", 2)) {
				pElixir->IsBuffAIRunning = false;
				WriteChatf("elixir target ai is now disabled");
				return;
			}
			WriteChatf("elixir target ai [0|1] (\a%s\ax): set smart buff AI values", (pElixir->IsTargetAIRunning ? "g1" : "r0"));
			return;
		}

		WriteChatf("elixir <target> [ai] (\a%s\ax): set auto target AI threshold", (pElixir->IsTargetAIRunning ? "g1" : "r0"));
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


void DrawElixirUI()
{
	char szBuffer[MAX_STRING] = { 0 };
	bool isChanged = false;
	bool isOpen = false;
	ImU32 redColor = IM_COL32(255, 100, 100, 100);
	ImU32 greenColor = IM_COL32(100, 255, 100, 100);
	ImU32 blueColor = IM_COL32(0, 0, 255, 100);
	ImU32 blueActiveColor = IM_COL32(20, 20, 255, 100);
	ImU32 blueHoverColor = IM_COL32(40, 40, 255, 100);
	ImGui::PushStyleColor(ImGuiCol_Text, (pElixir->IsElixirRunning) ? greenColor : redColor);
	ImGui::PushStyleColor(ImGuiCol_Tab, blueColor);
	ImGui::PushStyleColor(ImGuiCol_TabHovered, blueHoverColor);
	ImGui::PushStyleColor(ImGuiCol_TabActive, blueActiveColor);

	if (ImGui::Checkbox("Elixir AI", &pElixir->IsElixirRunning)) {
		isChanged = true;
	}
	ImGui::PopStyleColor(4);
	ImGui::SameLine();
	mq::imgui::HelpMarker("This is a global Elixir AI toggle. Turning this off disables all plugin actions.");

	if (pElixir->IsElixirRunning) {
		ImGui::PushStyleColor(ImGuiCol_Text, (pElixir->IsElixirDisabledOnFocus) ? greenColor : redColor);
		ImGui::PushStyleColor(ImGuiCol_Tab, blueColor);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, blueHoverColor);
		ImGui::PushStyleColor(ImGuiCol_TabActive, blueActiveColor);

		if (ImGui::Checkbox("Disable AI On Focus", &pElixir->IsElixirDisabledOnFocus)) {
			isChanged = true;
		}
		ImGui::PopStyleColor(4);
		ImGui::SameLine();
		mq::imgui::HelpMarker("When this EQ window is focused, should Elixir continue running?\nHelpful if you like to tab and take over this character.\nThis means AI will only run when this EQ window is in background.");

		ImGui::PushStyleColor(ImGuiCol_Text, (pElixir->IsBuffAIRunning) ? greenColor : redColor);
		ImGui::PushStyleColor(ImGuiCol_Tab, blueColor);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, blueHoverColor);
		ImGui::PushStyleColor(ImGuiCol_TabActive, blueActiveColor);
		sprintf_s(szBuffer, "Buff AI (%s)", (pElixir->IsBuffAIRunning) ? "Enabled" : "Disabled");
		isOpen = ImGui::TreeNode("##elixir.buffAILabel", szBuffer);
		ImGui::PopStyleColor(4);
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::EndPopup();
		}
		if (isOpen)
		{
			if (ImGui::Checkbox("Buff AI", &pElixir->IsBuffAIRunning)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("This AI will keep self buffs or target buffs on self maintained.\nIt is not smart enough yet to manage buffs on other targets, nor is it smart about stacking and such, beyond MQ2 built in support logic, buff stacking in EQ is complex and weird.");

			if (ImGui::Checkbox("During Combat", &pElixir->IsBuffDuringCombat)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("If Checked, Buffs will be casted even during combat.");
			ImGui::TreePop();
		}

		ImGui::PushStyleColor(ImGuiCol_Text, (pElixir->IsHealAIRunning) ? greenColor : redColor);
		ImGui::PushStyleColor(ImGuiCol_Tab, blueColor);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, blueHoverColor);
		ImGui::PushStyleColor(ImGuiCol_TabActive, blueActiveColor);

		if (!pElixir->IsHealAIRunning) {
			sprintf_s(szBuffer, "Heal AI (Disabled)");
		}
		else {
			sprintf_s(szBuffer, "Heal AI (Enabled <= %d%%%% HP)", pElixir->HealAIMax);
		}
		isOpen = ImGui::TreeNode("##elixir.healAILabel", szBuffer);
		ImGui::PopStyleColor(4);
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::EndPopup();
		}
		if (isOpen)
		{
			if (ImGui::Checkbox("Heal AI", &pElixir->IsHealAIRunning)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("This AI will attempt to use healing spells when you or a group mate reaches the % hp defined below.");

			if (ImGui::SliderInt("##elixir.healAIMax", &pElixir->HealAIMax, 10, 90, "Heal when less than %d%% HP")) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("Heal AI will trigger the lower number gem slot that is valid for the situation when a group mate hits this percent");

			ImGui::TreePop();
		}

		ImGui::PushStyleColor(ImGuiCol_Text, (pElixir->IsTargetAIRunning) ? greenColor : redColor);
		ImGui::PushStyleColor(ImGuiCol_Tab, blueColor);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, blueHoverColor);
		ImGui::PushStyleColor(ImGuiCol_TabActive, blueActiveColor);
		if (!pElixir->IsTargetAIRunning) {
			sprintf_s(szBuffer, "Target AI (Disabled)");
		}
		else {
			sprintf_s(szBuffer, "Target AI (Enabled <= %d distance)", pElixir->TargetAIMinRange);
		}
		isOpen = ImGui::TreeNode("##elixir.targetAILabel", szBuffer);
		ImGui::PopStyleColor(4);
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::EndPopup();
		}
		if (isOpen)
		{
			if (ImGui::Checkbox("AutoTarget AI", &pElixir->IsTargetAIRunning)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("This AI will automatically target the group assist player when it reaches min range and is line of sight.\nBe sure to mark an ally with the Main Assist role to have this properly work.");

			if (ImGui::SliderInt("##elixir.targetAIMax", &pElixir->TargetAIMinRange, 10, 200, "Target at %d distance")) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("Range of main assist target to begin assisting");

			if (ImGui::Checkbox("Auto Attack", &pElixir->IsTargetAutoAttack)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("AI will try to turn on auto attack after assisting");

			if (ImGui::Checkbox("Pet Attack", &pElixir->IsTargetPetAttack)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("AI will try to pet attack after assisting");

			ImGui::TreePop();
		}


		ImGui::PushStyleColor(ImGuiCol_Text, (pElixir->IsHateAIRunning) ? greenColor : redColor);
		ImGui::PushStyleColor(ImGuiCol_Tab, blueColor);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, blueHoverColor);
		ImGui::PushStyleColor(ImGuiCol_TabActive, blueActiveColor);
		if (!pElixir->IsHateAIRunning) {
			sprintf_s(szBuffer, "StopHate AI (Disabled)");
		}
		else {
			sprintf_s(szBuffer, "StopHate AI (Enabled >= %d%%%% aggro)", pElixir->HateAIMax);
		}
		isOpen = ImGui::TreeNode("##elixir.hateAILabel", szBuffer);
		ImGui::PopStyleColor(4);
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::EndPopup();
		}
		if (isOpen)
		{
			if (ImGui::Checkbox("StopHate AI", &pElixir->IsHateAIRunning)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("This AI will stop casting spells and offensive abilities if hate reaches this %\nNote this only works in group settings, if not in a group, ignored");

			if (ImGui::SliderInt("##elixir.hateAIMin", &pElixir->HateAIMax, 10, 95, "StopHate at %d%% aggro")) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("StopHate AI will stop casting offensive spells at this %%");

			ImGui::TreePop();
		}


		ImGui::PushStyleColor(ImGuiCol_Text, (pElixir->IsNukeAIRunning) ? greenColor : redColor);
		ImGui::PushStyleColor(ImGuiCol_Tab, blueColor);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, blueHoverColor);
		ImGui::PushStyleColor(ImGuiCol_TabActive, blueActiveColor);
		if (!pElixir->IsNukeAIRunning) {
			sprintf_s(szBuffer, "Nuke AI (Disabled)");
		}
		else {
			sprintf_s(szBuffer, "Nuke AI (Enabled <= %d %%HP %d %%Mana)", pElixir->NukeAIHPMax, pElixir->NukeAIManaMax);
		}
		isOpen = ImGui::TreeNode("##elixir.nukeAILabel", szBuffer);
		ImGui::PopStyleColor(4);
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::EndPopup();
		}
		if (isOpen)
		{
			if (ImGui::Checkbox("Nuke AI", &pElixir->IsNukeAIRunning)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("This AI will cast detrimental spells (not just nukes, dots, snares, slows etc too) and offensive abilities once a target is at %% hp");

			if (ImGui::SliderInt("##elixir.nukeAIMin", &pElixir->NukeAIHPMax, 10, 95, "Nuke at %d%% HP")) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("Nuke AI will not cast until target is at %");

			if (ImGui::SliderInt("##elixir.nukeAIManaMax", &pElixir->NukeAIManaMax, 10, 95, "Nuke until %d%% Mana")) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("Nuke AI will not cast once at % mana");
			ImGui::TreePop();
		}


		ImGui::PushStyleColor(ImGuiCol_Text, (pElixir->IsMeditateAIRunning) ? greenColor : redColor);
		ImGui::PushStyleColor(ImGuiCol_Tab, blueColor);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, blueHoverColor);
		ImGui::PushStyleColor(ImGuiCol_TabActive, blueActiveColor);
		if (!pElixir->IsMeditateAIRunning) {
			sprintf_s(szBuffer, "Meditate AI (Disabled)");
		}
		else {
			sprintf_s(szBuffer, "Meditate AI (Enabled Sit)");
		}
		isOpen = ImGui::TreeNode("##elixir.meditateAILabel", szBuffer);
		ImGui::PopStyleColor(4);
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::EndPopup();
		}
		if (isOpen)
		{
			if (ImGui::Checkbox("Meditate AI", &pElixir->IsMeditateAIRunning)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("This AI will attempt to meditate when the situation seems proper.");

			if (ImGui::Checkbox("During Combat", &pElixir->IsMeditateEnabledDuringCombat)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("AI will try to sit even when combat is detected?");

			if (ImGui::Checkbox("By Sitting", &pElixir->IsMeditateBySitting)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("Try to sit down when it seems a safe time to.");

			/*
			if (ImGui::Checkbox("By Cannibalize", &pElixir->IsMeditateByCannibalize)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("Use available canni spells to attempt to regenerate mana");

			if (ImGui::SliderInt("##elixir.meditateAICanniManaMax", &pElixir->CanniMaxMana, 0, 99, "Canni when below %d%% mana")) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("Attempt to Cannibalize when mana is below provided amount");

			if (ImGui::SliderInt("##elixir.meditateAICanniMinHP", &pElixir->CanniMinHP, 0, 99, "Canni until at %d%% HP")) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("Attempt to Cannibalize but if HP reaches this amount, stop");
			*/
			ImGui::TreePop();
		}

		ImGui::PushStyleColor(ImGuiCol_Text, (pElixir->IsCharmAIRunning) ? greenColor : redColor);
		ImGui::PushStyleColor(ImGuiCol_Tab, blueColor);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, blueHoverColor);
		ImGui::PushStyleColor(ImGuiCol_TabActive, blueActiveColor);
		if (!pElixir->IsCharmAIRunning) {
			sprintf_s(szBuffer, "Charm AI (Disabled)");
		}
		else {
			sprintf_s(szBuffer, "Charm AI (Enabled - %s)", pElixir->CharmName);
		}
		isOpen = ImGui::TreeNode("##elixir.charmAILabel", szBuffer);
		ImGui::PopStyleColor(4);
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::EndPopup();
		}
		if (isOpen)
		{
			if (ImGui::Checkbox("Charm AI", &pElixir->IsCharmAIRunning)) {
				isChanged = true;
			}
			ImGui::SameLine();
			mq::imgui::HelpMarker("This AI will attempt to charm a designated target as a high priority process.");

			ImGui::PushStyleColor(ImGuiCol_Text, (pElixir->CharmName != "No Target\0") ? greenColor : redColor);
			ImGui::PushStyleColor(ImGuiCol_Tab, blueColor);
			ImGui::PushStyleColor(ImGuiCol_TabHovered, blueHoverColor);
			ImGui::PushStyleColor(ImGuiCol_TabActive, blueActiveColor);
			ImGui::Text("Charm Target: %s", pElixir->CharmName);
			ImGui::PopStyleColor(4);

			if (!pElixir->IsCharmCurrentValid) ImGui::BeginDisabled();
			if (ImGui::Button("Set Charm Target")) {
				pElixir->CharmSpawnID = pTarget->SpawnID;
				sprintf_s(pElixir->CharmName, pTarget->Name);
			}
			if (!pElixir->IsCharmCurrentValid) ImGui::EndDisabled();
			ImGui::SameLine();
			mq::imgui::HelpMarker("AI will try to keep charm target charmed");

			bool isClearDisabled = pElixir->CharmSpawnID == 0;
			if (isClearDisabled) ImGui::BeginDisabled();
			if (ImGui::Button("Clear Charm Target")) {
				pElixir->ClearCharmTarget();
			}
			if (isClearDisabled) ImGui::EndDisabled();
			ImGui::SameLine();
			mq::imgui::HelpMarker("Clear current charmed target, disabling charm AI until a new target is set");
			
			ImGui::TreePop();
		}

		ImGui::PushStyleColor(ImGuiCol_Text, (pElixir->ignoreGemCount > 0) ? greenColor : redColor);
		ImGui::PushStyleColor(ImGuiCol_Tab, blueColor);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, blueHoverColor);
		ImGui::PushStyleColor(ImGuiCol_TabActive, blueActiveColor);

		sprintf_s(szBuffer, "Ignored Gems (%d/%d)", pElixir->ignoreGemCount, pElixir->maxGemCount);
		isOpen = ImGui::TreeNode("##elixir.ignoreGemLabel", szBuffer);
		ImGui::PopStyleColor(4);
		if (ImGui::BeginPopupContextItem())
		{
			ImGui::EndPopup();
		}
		if (isOpen)
		{
			for (int i = 0; i < pElixir->maxGemCount; i++) {
				sprintf_s(szBuffer, "Ignore Gem %d (no spell)", i + 1);
				if (int spellID = GetMemorizedSpell(i)) {
					if (PSPELL pSpell = GetSpellByID(spellID)) {
						sprintf_s(szBuffer, "Ignore Gem %d (%s)", i + 1, pSpell->Name);
					}
				}

				if (ImGui::Checkbox(szBuffer, &pElixir->IgnoreGems[i])) {
					isChanged = true;
				}
				ImGui::SameLine();
				mq::imgui::HelpMarker("Gem will be ignored and no AI will skip trying to cast it");
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		ImGui::Checkbox("Debug", &pElixir->IsSettingsDebugEnabled);
		if (pElixir->IsSettingsDebugEnabled) {
			ImGui::Text("LastAction: %s", pElixir->LastAction.c_str());
			ImGui::Text("Target AI: %s", pElixir->TargetAIStr.c_str());
			ImGui::Text("Charm AI: %s", pElixir->CharmAIStr.c_str());
			ImGui::Text("Meditate AI: %s", pElixir->MeditateAIStr.c_str());
			for (int i = 0; i < 12; i++) {
				ImGui::Text("Button %d: %s", i + 1, pElixir->Buttons[i].c_str());
			}
		}
	}

	if (isChanged) {
		SaveINI();
	}
}

void DrawElixirUIPanel()
{
	if (ImGui::Button("Toggle Elixir UI")) {
		pElixir->IsElixirUIShowing = !pElixir->IsElixirUIShowing;
	}
	ImGui::SameLine();
	mq::imgui::HelpMarker("Typing in /elixir ui is a shortcut to this button");
}