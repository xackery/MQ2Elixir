// MQ2Elixir is a player assistance plugin


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

void CheckElixir()
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) return;
	if (!pChar->pSpawn) return;

	int lastStance;
	if (pElixir) lastStance = pElixir->StanceMode;
	
	if (pElixir && pChar->pSpawn->GetClass() != pElixir->Class()) {
		//delete elixir;

		//check for class change (loaded into a new character)
		char szName[256] = { 0 };
		strcpy_s(szName, pChar->Name);
		lastStance = GetPrivateProfileInt(szName, "Options-DefaultStanceMode", 0, INIFileName);
		
	}

	if (!pElixir) {
		switch (pChar->pSpawn->GetClass()) {
		case Bard:
			pElixir = new BardElixir();
		case Beastlord:
			pElixir = new BeastlordElixir();
		case Berserker:
			pElixir = new BerserkerElixir();
		case Cleric:
			pElixir = new ClericElixir();
		case Druid:
			pElixir = new DruidElixir();
		case Enchanter:
			pElixir = new EnchanterElixir();
		case Mage:
			pElixir = new MageElixir();
		case Monk:
			pElixir = new MonkElixir();
		case Necromancer:
			pElixir = new NecromancerElixir();
		case Paladin:
			pElixir = new PaladinElixir();
		case Ranger:
			pElixir = new RangerElixir();
		case Rogue:
			pElixir = new RogueElixir();
		case Shadowknight:
			pElixir = new ShadowknightElixir();
		case Shaman:
			pElixir = new ShamanElixir();
		case Warrior:
			pElixir = new WarriorElixir();
		case Wizard:
			pElixir = new WizardElixir();
		default:
			//WriteChatf("MQ2Elixir::ElixirCheck unknown class %d, unloading", pChar->pSpawn->GetClass());
			//EzCommand("/timed 1 /plugin mq2elixir unload");
			return;
		}
		WriteChatf("Class changed to %d", pChar->pSpawn->GetClass());
		pElixir->StanceMode = lastStance;
	}
	if (!pElixir) return;
}

BOOL dataElixir(PCHAR szName, MQ2TYPEVAR& Dest) {
	Dest.DWord = 1;
	Dest.Type = pElixirType;
	return true;
}


// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
    DebugSpewAlways("Initializing MQ2Elixir");
	pElixir = nullptr;
	pElixirType = new MQ2ElixirType;
	AddMQ2Data("Elixir", dataElixir);

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
	CheckElixir();
	CheckWndActive();
	if (GetCharInfo()) {
		char szName[256] = { 0 };
		strcpy_s(szName, GetCharInfo()->Name);
		if (pElixir) pElixir->StanceMode = GetPrivateProfileInt(szName, "Options-DefaultStanceMode", 0, INIFileName);
	}


    //Add commands, MQ2Data items, hooks, etc.
    //AddCommand("/mycommand",MyCommand);
    //AddXMLFile("MQUI_MyXMLFile.xml");
    //bmMyBenchmark=AddMQ2Benchmark("My Benchmark Name");
}


bool MQ2ElixirType::GetMember(MQ2VARPTR VarPtr, char* Member, char* Index, MQ2TYPEVAR& Dest) {

	PMQ2TYPEMEMBER pMethod = MQ2ElixirType::FindMethod(Member);
	if (pMethod) {
		switch ((ElixirMethods)pMethod->ID) {
		case Cast:
			Dest.DWord = ActionCast(Index);
			Dest.Type = pBoolType;
			return true;
		case CastItem:
			Dest.DWord = ActionCastItem(Index);
			Dest.Type = pBoolType;
			return true;
		case CastSpell:
			Dest.DWord = ActionCastSpell(Index);
			Dest.Type = pBoolType;
			return true;
		case CastAbility:
			Dest.DWord = ActionCastAbility(Index);
			Dest.Type = pBoolType;
			return true;
		case CastCombatAbility:
			Dest.DWord = ActionCastCombatAbility(Index);
			Dest.Type = pBoolType;
			return true;
		case CastAA:
			Dest.DWord = ActionCastAA(Index);
			Dest.Type = pBoolType;
			return true;
		case Memorize:
			//TODO: split the gem from the index string
			//Dest.DWord = ActionMemorizeSpell(Index);
			Dest.DWord = false;
			Dest.Type = pBoolType;
			return true;
		}
	}

	PMQ2TYPEMEMBER pMember = MQ2ElixirType::FindMember(Member);
	if (!pMember) return false;

	switch (pMember->ID) {
	case Stance:
		int stance = -1;
		if (pElixir) stance = pElixir->StanceMode;
		Dest.DWord =  stance;
		Dest.Type = pIntType;
		return true;
	}

	return false;
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

	delete pElixirType;
	delete pElixir;

	RemoveMQ2Data("Elixir");

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
		if (pElixir)	ElixirLaunchWnd->CModeList->SetChoice(pElixir->StanceMode);

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
	
	CheckElixir();
	if (!pElixir) return;

	if (ElixirLaunchWnd && ElixirLaunchWnd->CModeList->GetCurChoice() != pElixir->StanceMode) {
		pElixir->StanceMode = ElixirLaunchWnd->CModeList->GetCurChoice();
		WriteChatf("[MQ2Elixir] Stance is now %d", pElixir->StanceMode);
	}
	PCHARINFO pChar = GetCharInfo();

	if (!pChar) return;	
	BuffUpdate(pChar->pSpawn);
	if (pTarget) {
		PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(pTarget->Data.SpawnID);
		if (pSpawn)	BuffUpdate(pSpawn);
	}

	if (pElixir->Logic()) return;
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
