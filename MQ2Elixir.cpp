// MQ2Elixir is a player assistance plugin


#include "../MQ2Plugin.h"
#include "MQ2Elixir.h"
#include "Core.h"
#include "Elixir.h"

PreSetup("MQ2Elixir");
PLUGIN_VERSION(0.2);


// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
{
	DebugSpewAlways("Initializing MQ2Elixir");
	pElixir = new Elixir();
	pElixirType = new MQ2ElixirType;
	AddMQ2Data("Elixir", dataElixir);
	
	if (GetCharInfo()) {
		char szName[256] = { 0 };
		strcpy_s(szName, GetCharInfo()->Name);
		pElixir->StanceMode = GetPrivateProfileInt(szName, "Options-DefaultStanceMode", 0, INIFileName);
	}

	//Add commands, MQ2Data items, hooks, etc.
	AddCommand("/elixir", ElixirCommand);
}

BOOL dataElixir(PCHAR szName, MQ2TYPEVAR& Dest) {
	Dest.DWord = 1;
	Dest.Type = pElixirType;
	return true;
}

bool MQ2ElixirType::GetMember(MQ2VARPTR VarPtr, char* Member, char* Index, MQ2TYPEVAR& Dest) {

	PMQ2TYPEMEMBER pMethod = MQ2ElixirType::FindMethod(Member);
	if (pMethod) {
		if ((ElixirMethods)pMethod->ID == Cast)
		{
			Dest.DWord = ActionCast(Index);
			Dest.Type = pBoolType;
			return true;
		}
		if ((ElixirMethods)pMethod->ID == CastItem)
		{
			Dest.DWord = ActionCastItem(Index);
			Dest.Type = pBoolType;
			return true;
		}
		if ((ElixirMethods)pMethod->ID == CastSpell)
		{
			Dest.DWord = ActionCastSpell(Index);
			Dest.Type = pBoolType;
			return true;
		}
		if ((ElixirMethods)pMethod->ID == CastAbility)
		{
			Dest.DWord = ActionCastAbility(Index);
			Dest.Type = pBoolType;
			return true;
		}
		if ((ElixirMethods)pMethod->ID == CastCombatAbility)
		{
			Dest.DWord = ActionCastCombatAbility(Index);
			Dest.Type = pBoolType;
			return true;
		}
		if ((ElixirMethods)pMethod->ID == CastAA)
		{
			Dest.DWord = ActionCastAA(Index);
			Dest.Type = pBoolType;
			return true;
		}
		if ((ElixirMethods)pMethod->ID == Memorize)
		{
			char szName[256] = { 0 };
			PSPAWNINFO pSpawn = nullptr;
			WORD gem = 0;
			int counter = 0;
			char* token = strtok(Index, " ");
			std::string name;

			while (token != NULL)
			{
				counter++;
				if (counter == 1) {
					gem = atoi(token);
				}
				else {
					name.append(token);
				}
				token = strtok(NULL, " ");
				if (counter > 1 && token != NULL) name.append(" ");
			}

			Dest.DWord = ActionMemorizeSpell(gem, (PCHAR)name.c_str());

			Dest.DWord = false;
			Dest.Type = pBoolType;
			return true;
		}
	}

	PMQ2TYPEMEMBER pMember = MQ2ElixirType::FindMember(Member);
	if (!pMember) return false;

	PSPAWNINFO pSpawn = nullptr;
	int stance = -1;

	if (pMember->ID == Stance) {
		if (pElixir) stance = pElixir->StanceMode;
		Dest.DWord = stance;
		Dest.Type = pIntType;
		return true;
	}
	if (pMember->ID == IsFacingTarget) {
		Dest.DWord = false;
		pSpawn = (PSPAWNINFO)GetSpawnByID(pTarget->Data.SpawnID);
		if (pSpawn) Dest.DWord = IsFacingSpawn(pSpawn);
		Dest.Type = pBoolType;
		return true;
	}
	if (pMember->ID == TargetHasBuff) {
		Dest.DWord = false;
		pSpawn = (PSPAWNINFO)GetSpawnByID(pTarget->Data.SpawnID);
		if (pSpawn) Dest.DWord = HasBuff(pSpawn, Index);
		Dest.Type = pBoolType;
		return true;
	}
	if (pMember->ID == SpawnIDHasBuff) {
		Dest.DWord = false;
		std::string name;
		PSPAWNINFO pSpawn = nullptr;

		int counter = 0;
		char* token = strtok(Index, " ");
		while (token != NULL)
		{
			counter++;
			if (counter == 1) {
				int spawnID = atoi(token);
				if (!spawnID) return true;
				pSpawn = (PSPAWNINFO)GetSpawnByID(spawnID);
				if (!pSpawn) return true;
			}
			else {
				name.append(token);
			}
			token = strtok(NULL, " ");
			if (token != NULL && counter > 1) name.append(" ");
		}

		if (!name.size()) return true;
		if (!pSpawn) return true;

		Dest.DWord = HasBuff(pSpawn, (PCHAR)name.c_str());
		Dest.Type = pBoolType;
		return true;
	}
	if (pMember->ID == LastAction) {
		strcpy_s(lastAction, pElixir->LastAction.c_str());
		Dest.Type = pStringType;
		Dest.Ptr = lastAction;
		return true;
	}
	if (pMember->ID == Gem1) {
		strcpy_s(gem1, pElixir->Gems[0].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem1;
		return true;
	}
	if (pMember->ID == Gem2) {
		strcpy_s(gem2, pElixir->Gems[1].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem2;
		return true;
	}
	if (pMember->ID == Gem3) {
		strcpy_s(gem3, pElixir->Gems[2].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem3;
		return true;
	}
	if (pMember->ID == Gem4) {
		strcpy_s(gem4, pElixir->Gems[3].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem4;
		return true;
	}
	if (pMember->ID == Gem5) {
		strcpy_s(gem5, pElixir->Gems[4].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem5;
		return true;
	}
	if (pMember->ID == Gem6) {
		strcpy_s(gem6, pElixir->Gems[5].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem6;
		return true;
	}
	if (pMember->ID == Gem7) {
		strcpy_s(gem7, pElixir->Gems[6].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem7;
		return true;
	}
	if (pMember->ID == Gem8) {
		strcpy_s(gem8, pElixir->Gems[7].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem8;
		return true;
	}
	if (pMember->ID == Gem9) {
		strcpy_s(gem9, pElixir->Gems[8].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem9;
		return true;
	}
	if (pMember->ID == Gem10) {
		strcpy_s(gem10, pElixir->Gems[9].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem10;
		return true;
	}
	if (pMember->ID == Gem11) {
		strcpy_s(gem11, pElixir->Gems[10].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem11;
		return true;
	}
	if (pMember->ID == Gem12) {
		strcpy_s(gem12, pElixir->Gems[11].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem12;
		return true;
	}
	if (pMember->ID == Gem13) {
		strcpy_s(gem13, pElixir->Gems[12].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = gem13;
		return true;
	}
	if (pMember->ID == Button1) {
		strcpy_s(button1, pElixir->Buttons[0].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button1;
		return true;
	}
	if (pMember->ID == Button2) {
		strcpy_s(button2, pElixir->Buttons[1].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button2;
		return true;
	}
	if (pMember->ID == Button3) {
		strcpy_s(button3, pElixir->Buttons[2].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button3;
		return true;
	}
	if (pMember->ID == Button4) {
		strcpy_s(button4, pElixir->Buttons[3].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button4;
		return true;
	}
	if (pMember->ID == Button5) {
		strcpy_s(button5, pElixir->Buttons[4].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button5;
		return true;
	}
	if (pMember->ID == Button6) {
		strcpy_s(button6, pElixir->Buttons[5].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button6;
		return true;
	}
	if (pMember->ID == Button7) {
		strcpy_s(button7, pElixir->Buttons[6].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button7;
		return true;
	}
	if (pMember->ID == Button8) {
		strcpy_s(button8, pElixir->Buttons[7].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button8;
		return true;
	}
	if (pMember->ID == Button9) {
		strcpy_s(button9, pElixir->Buttons[8].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button9;
		return true;
	}
	if (pMember->ID == Button10) {
		strcpy_s(button10, pElixir->Buttons[9].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button10;
		return true;
	}
	if (pMember->ID == Button11) {
		strcpy_s(button11, pElixir->Buttons[10].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button11;
		return true;
	}
	if (pMember->ID == Button12) {
		strcpy_s(button12, pElixir->Buttons[11].c_str());
		Dest.Type = pStringType;
		Dest.Ptr = button12;
		return true;
	}
	return false;
}

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
    DebugSpewAlways("Shutting down MQ2Elixir");
	
	delete pElixirType;
	delete pElixir;

	RemoveMQ2Data("Elixir");
    RemoveCommand("/elixir");
}

// Called after entering a new zone
PLUGIN_API VOID OnZoned(VOID)
{
 //   DebugSpewAlways("MQ2Elixir::OnZoned()");
	pElixir->ZoneCooldown = (unsigned long)MQGetTickCount64() + 5000;
}

// Called once directly before shutdown of the new ui system, and also
// every time the game calls CDisplay::CleanGameUI()
PLUGIN_API VOID OnCleanUI(VOID)
{
}

// Called once directly after the game ui is reloaded, after issuing /loadskin
PLUGIN_API VOID OnReloadUI(VOID)
{
	if (gGameState != GAMESTATE_INGAME) return;
	if (!pCharSpawn) return;
}

// Called every frame that the "HUD" is drawn -- e.g. net status / packet loss bar
PLUGIN_API VOID OnDrawHUD(VOID)
{
    // DONT leave in this debugspew, even if you leave in all the others
    //DebugSpewAlways("MQ2Elixir::OnDrawHUD()");
}

// Called once directly after initialization, and then every time the gamestate changes
PLUGIN_API VOID SetGameState(DWORD GameState)
{
	if (GameState != GAMESTATE_INGAME) return;
}

// This is called every time MQ pulses
PLUGIN_API VOID OnPulse(VOID)
{
	if (GetGameState() != GAMESTATE_INGAME) {
		return;
	}

	if (PulseDelay > MQGetTickCount64()) {
		return;
	}

	PulseDelay = MQGetTickCount64() + 1000;
	
	PCHARINFO pChar = GetCharInfo();

	if (!pChar) return;	
	BuffUpdate(pChar->pSpawn);
	if (pTarget) {
		PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(pTarget->Data.SpawnID);
		if (pSpawn) {
			BuffUpdate(pSpawn);
		}
	}

	pElixir->OnPulse();
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
}

// This is called when we receive the EQ_END_ZONE packet is received
PLUGIN_API VOID OnEndZone(VOID)
{
	IsZoning = false;
}

// This is called when pChar!=pCharOld && We are NOT zoning
// honestly I have no idea if its better to use this one or EndZone (above)
PLUGIN_API VOID Zoned(VOID)
{
//    DebugSpewAlways("MQ2Elixir::Zoned");
}

PLUGIN_API void ElixirCommand(PSPAWNINFO pLPlayer, char* szLine)
{
	if (!GetCharInfo2()) return;

	char szArg1[MAX_STRING] = { 0 };
	char szArg2[MAX_STRING] = { 0 };
	GetArg(szArg1, szLine, 1);
	GetArg(szArg2, szLine, 2);

	if (!_strnicmp(szArg1, "list", 4) || !_strnicmp(szArg2, "list", 4)) {
		return;
	}
	if (!_strnicmp(szArg1, "help", 4)) {
		//Help();
		return;
	}
}
