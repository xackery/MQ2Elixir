// MQ2Elixir.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.



#include "../MQ2Plugin.h"
#include "MQ2Elixir.h"
#include "Core.h"
#include "BardElixir.h"
#include "BaseElixir.h"
#include "BeastlordElixir.h"
#include "BerserkerElixir.h"
#include "ClericElixir.h"
#include "DruidElixir.h"
#include "EnchanterElixir.h"
#include "MageElixir.h"
#include "MonkElixir.h"
#include "NecromancerElixir.h"
#include "PaladinElixir.h"
#include "RangerElixir.h"
#include "RogueElixir.h"
#include "ShadowknightElixir.h"
#include "ShamanElixir.h"
#include "WarriorElixir.h"
#include "WizardElixir.h"

PreSetup("MQ2Elixir");
PLUGIN_VERSION(0.1);

void ElixirCheck()
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) return;
	if (!pChar->pSpawn) return;

	int lastStance;
	if (elixir) lastStance = elixir->StanceMode;

	if (elixir && pChar->pSpawn->GetClass() != elixir->Class()) {
		delete elixir;

		//check for class change (loaded into a new character)
		if (GetCharInfo()) {
			char szName[256] = { 0 };
			strcpy_s(szName, GetCharInfo()->Name);
			lastStance = GetPrivateProfileInt(szName, "Options-DefaultStanceMode", 0, INIFileName);
		}
	}

	if (elixir == NULL) {
		switch (pChar->pSpawn->GetClass()) {
		case Bard:
			elixir = new BardElixir();
		case Beastlord:
			elixir = new BeastlordElixir();
		case Berserker:
			elixir = new BerserkerElixir();
		case Cleric:
			elixir = new ClericElixir();
		case Druid:
			elixir = new DruidElixir();
		case Enchanter:
			elixir = new EnchanterElixir();
		case Mage:
			elixir = new MageElixir();
		case Monk:
			elixir = new MonkElixir();
		case Necromancer:
			elixir = new NecromancerElixir();
		case Paladin:
			elixir = new PaladinElixir();
		case Ranger:
			elixir = new RangerElixir();
		case Rogue:
			elixir = new RogueElixir();
		case Shadowknight:
			elixir = new ShadowknightElixir();
		case Shaman:
			elixir = new ShamanElixir();
		case Warrior:
			elixir = new WarriorElixir();
		case Wizard:
			elixir = new WizardElixir();
		default:
			WriteChatf("MQ2Elixir::ElixirCheck unknown class, unloading");
			EzCommand("/timed 1 /plugin mq2autosize unload");
			return;
		}
		if (pChar->pSpawn->GetClass() == Shaman) 
		if (pChar->pSpawn->GetClass() == Shadowknight) elixir = new ShadowknightElixir();
		if (pChar->pSpawn->GetClass() == Cleric) elixir = new ClericElixir();
		elixir->StanceMode = lastStance;
	}
	if (elixir == NULL) return;
}

// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
    DebugSpewAlways("Initializing MQ2Elixir");

	if (IsXMLFilePresent("MQUI_ElixirLaunchWnd.xml")) {
		AddXMLFile("MQUI_ElixirLaunchWnd.xml");
	} else {
		WriteChatf("[MQ2Elixir] could not find MQUI_ElixirLaunchWnd.xml.  Please place in uifiles/default");
	}
	if (IsXMLFilePresent("MQUI_ElixirOptionsWnd.xml")) {
		AddXMLFile("MQUI_ElixirOptionsWnd.xml");
	}
	else {
		WriteChatf("[MQ2Elixir] could not find MQUI_ElixirOptionsWnd.xml.  Please place in uifiles/default");
	}
	ElixirCheck();
	CheckWndActive();
	if (GetCharInfo()) {
		char szName[256] = { 0 };
		strcpy_s(szName, GetCharInfo()->Name);
		if (elixir) elixir->StanceMode = GetPrivateProfileInt(szName, "Options-DefaultStanceMode", 0, INIFileName);
	}


    //Add commands, MQ2Data items, hooks, etc.
    //AddCommand("/mycommand",MyCommand);
    //AddXMLFile("MQUI_MyXMLFile.xml");
    //bmMyBenchmark=AddMQ2Benchmark("My Benchmark Name");
}


void CheckWndActive() 
{
	IsElixirLaunchWndActive = false;
	IsElixirOptionsWndActive = false;
	if (IsZoning) return;
	if (IsWrongUI) return;

	if (ElixirLaunchWnd && !ElixirLaunchWnd->IsVisible()) IsElixirLaunchWndActive = true;
	if (ElixirOptionsWnd && !ElixirOptionsWnd->IsVisible()) IsElixirOptionsWndActive = true;
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2Elixir");
	DestroyElixirWindows();

	// Additions End

    //Remove commands, MQ2Data items, hooks, etc.
    //RemoveMQ2Benchmark(bmMyBenchmark);
    //RemoveCommand("/mycommand");
    //RemoveXMLFile("MQUI_MyXMLFile.xml");
}

// Called after entering a new zone
PLUGIN_API VOID OnZoned(VOID)
{
 //   DebugSpewAlways("MQ2Elixir::OnZoned()");
}

void DestroyElixirWindows() {
	if (ElixirOptionsWnd) {
		ElixirOptionsWnd->SaveLoc();
		delete ElixirOptionsWnd;
		ElixirOptionsWnd = 0;
	}
	if (ElixirLaunchWnd) {
		ElixirLaunchWnd->SaveLoc();
		delete ElixirLaunchWnd;
		ElixirLaunchWnd = 0;
	}
	CheckWndActive();
}


// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API VOID OnCleanUI(VOID)
{
	DestroyElixirWindows();
}

// Called once directly after the game ui is reloaded, after issuing /loadskin
PLUGIN_API VOID OnReloadUI(VOID)
{
	if (gGameState != GAMESTATE_INGAME) return;
	if (!pCharSpawn) return;
	CreateElixirWindows();
}

// Called every frame that the "HUD" is drawn -- e.g. net status / packet loss bar
PLUGIN_API VOID OnDrawHUD(VOID)
{
    // DONT leave in this debugspew, even if you leave in all the others
    //DebugSpewAlways("MQ2Elixir::OnDrawHUD()");
}

void CreateElixirWindows() {
	if (ElixirLaunchWnd && ElixirOptionsWnd) return;

	if (pSidlMgr->FindScreenPieceTemplate("ElixirOptionsWnd")) {
		ElixirOptionsWnd = new CElixirOptionsWnd((CXWnd*)ElixirLaunchWnd);
		if (ElixirOptionsWnd->IsVisible()) ((CXWnd*)ElixirOptionsWnd)->Show(1, 1);
		
		char szTitle[MAX_STRING];
		sprintf_s(szTitle, "Elixir v%.1f Options", MQ2Version);
		ElixirOptionsWnd->CSetWindowText(szTitle);
	}
	if (pSidlMgr->FindScreenPieceTemplate("ElixirLaunchWnd")) {
		ElixirLaunchWnd = new CElixirLaunchWnd((CXWnd*)ElixirOptionsWnd);
		if (elixir)	ElixirLaunchWnd->CModeList->SetChoice(elixir->StanceMode);

		if (GetCharInfo()) {
			char szName[256] = { 0 };
			strcpy_s(szName, GetCharInfo()->Name);
			bool isSaved = (GetPrivateProfileInt(szName, "Saved", 0, INIFileName) > 0 ? true : false);
			bool isLaunchEnabled = true;
			if (isSaved) {
				isLaunchEnabled = (GetPrivateProfileInt(szName, "Options-EnableLaunch", 0, INIFileName) > 0 ? true : false);
			}
			if (ElixirLaunchWnd->IsVisible() && isLaunchEnabled) ((CXWnd*)ElixirLaunchWnd)->Show(1, 1);
		}
	}
	CheckWndActive();
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
	if (GameState != GAMESTATE_INGAME) return;
	CreateElixirWindows();
}

// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
	if (GetGameState() != GAMESTATE_INGAME) return;
	

	if (PulseDelay > MQGetTickCount64()) return;
	PulseDelay = MQGetTickCount64() + 3000;
	
	ElixirCheck();
	if (!elixir) return;

	if (ElixirLaunchWnd && ElixirLaunchWnd->CModeList->GetCurChoice() != elixir->StanceMode) {
		elixir->StanceMode = ElixirLaunchWnd->CModeList->GetCurChoice();
		WriteChatf("[MQ2Elixir] Stance is now %d", elixir->StanceMode);
	}
	PCHARINFO pChar = GetCharInfo();

	if (!pChar) return;	
	BuffUpdate(pChar->pSpawn);
	if (pTarget) {
		PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(pTarget->Data.SpawnID);
		if (pSpawn)	BuffUpdate(pSpawn);
	}

	if (elixir->Logic()) return;
}

// This is called every time WriteChatColor is called by MQ2Main or any plugin,
// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT
// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE (for chat plugins)
PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
{
  //  DebugSpewAlways("MQ2Elixir::OnWriteChatColor(%s)",Line);
    return 0;
}

// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
// but after MQ filters and chat events are taken care of.
PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
{
   // DebugSpewAlways("MQ2Elixir::OnIncomingChat(%s)",Line);
    return 0;
}

// This is called each time a spawn is added to a zone (inserted into EQ's list of spawns),
// or for each existing spawn when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddSpawn(PSPAWNINFO pNewSpawn)
{
   // DebugSpewAlways("MQ2Elixir::OnAddSpawn(%s)",pNewSpawn->Name);
}

// This is called each time a spawn is removed from a zone (removed from EQ's list of spawns).
// It is NOT called for each existing spawn when a plugin shuts down.
PLUGIN_API VOID OnRemoveSpawn(PSPAWNINFO pSpawn)
{
 //   DebugSpewAlways("MQ2Elixir::OnRemoveSpawn(%s)",pSpawn->Name);
}

// This is called each time a ground item is added to a zone
// or for each existing ground item when a plugin first initializes
// NOTE: When you zone, these will come BEFORE OnZoned
PLUGIN_API VOID OnAddGroundItem(PGROUNDITEM pNewGroundItem)
{
    //DebugSpewAlways("MQ2Elixir::OnAddGroundItem(%d)",pNewGroundItem->DropID);
}

// This is called each time a ground item is removed from a zone
// It is NOT called for each existing ground item when a plugin shuts down.
PLUGIN_API VOID OnRemoveGroundItem(PGROUNDITEM pGroundItem)
{
   // DebugSpewAlways("MQ2Elixir::OnRemoveGroundItem(%d)",pGroundItem->DropID);
}

// This is called when we receive the EQ_BEGIN_ZONE packet is received
PLUGIN_API VOID OnBeginZone(VOID)
{
	IsZoning = true;
	CheckWndActive();
}

// This is called when we receive the EQ_END_ZONE packet is received
PLUGIN_API VOID OnEndZone(VOID)
{
	IsZoning = false;
	CheckWndActive();
}

// This is called when pChar!=pCharOld && We are NOT zoning
// honestly I have no idea if its better to use this one or EndZone (above)
PLUGIN_API VOID Zoned(VOID)
{
//    DebugSpewAlways("MQ2Elixir::Zoned");
}
