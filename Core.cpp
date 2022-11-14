#include <mq/Plugin.h>
#include "Core.h"
#include "../MQ2FarmTest/NavCommands.h"

using namespace std;

#if !defined(ROF2EMU) && !defined(UF2EMU)
// How many NPC XTargets within 250 feet of me are there?
int XTargetNearbyHaterCount()
{
	int count = 0;
	XTARGETSLOT xts;
	ExtendedTargetList* xtm = pLocalPC->pXTargetMgr;
	if (!xtm) return count;
	if (!pAggroInfo) return count;

	for (int i = 0; i < xtm->XTargetSlots.Count; i++) {
		xts = xtm->XTargetSlots[i];
		if (!xts.SpawnID) continue;
		if (xts.xTargetType != XTARGET_AUTO_HATER) continue;
		PSPAWNINFO pSpawn = (PSPAWNINFO)GetSpawnByID(xts.SpawnID);
		if (!pSpawn) continue;
		if (GetDistance(pLocalPlayer, pSpawn) > 250) continue;
		count++;
	}

	return count;
}
#endif

// Is spell casting ready to be done?
bool IsCastingReady() 
{
	if ((int)((CDisplay*)pDisplay)->TimeStamp <= ((PSPAWNINFO)pLocalPlayer)->GetSpellCooldownETA()) return false;

	return true;
}

// Is provided spell memorized and ready to be casted?
bool IsSpellReady(PCHAR szName)
{
	if (!IsCastingReady()) return false;
	

	string szSpellName = szName;
	PSPELL pSpell1 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell2 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell3 = GetSpellByName((char*)szSpellName.c_str());
	if (!pSpell1 && !pSpell2 && !pSpell3) {
		DebugSpewAlways("MQ2Elixir::IsSpellReady spell %s not found", szName);
		return false;
	}
	PSPELL pSpell = nullptr;

	for (int GEM = 0; GEM < NUM_SPELL_GEMS; GEM++) {
		if (pSpell1 && GetMemorizedSpell(GEM) == pSpell1->ID) pSpell = pSpell1;
		if (pSpell2 && GetMemorizedSpell(GEM) == pSpell2->ID) pSpell = pSpell2;
		if (pSpell3 && GetMemorizedSpell(GEM) == pSpell3->ID) pSpell = pSpell3;
		if (!pSpell) continue;
		if (((CDisplay*)pDisplay)->TimeStamp <= ((PSPAWNINFO)pLocalPlayer)->SpellGemETA[GEM]) continue;
		return true;
	}
	return false;
}

// IsIdealDamageReceiver means you likely should be doing damage in the current situation (you have aggro, there's a main tank, you're not a tank class, etc)
bool IsIdealDamageReceiver() 
{
	bool isMainTank = false;
	bool isAnyMainTank = false;
	bool isTauntClassInGroup = false;
	CGroupMember* pG;
	PSPAWNINFO pSpawn;

	for (int i = 0; i < 6; i++) {
		if (!pLocalPC->Group) continue;
		pG = pLocalPC->Group->GetGroupMember(i);
		if (!pG) continue;
		if (pG->Offline) continue;
		//if (${Group.Member[${i}].Mercenary}) /continue
		//if (${ Group.Member[${i}].OtherZone }) / continue
		pSpawn = pG->pSpawn;
		if (!pSpawn) continue;
		
		if (pSpawn->Type == SPAWN_CORPSE) continue;
		if (pSpawn->GetClass() == Shadowknight || 
			pSpawn->GetClass() == Paladin || 
			pSpawn->GetClass() == Warrior) {
			isTauntClassInGroup = true;
		}
		if (pG->MainTank != 0) continue;
		isAnyMainTank = true;
		isTauntClassInGroup = true;
		if (pSpawn->SpawnID != pLocalPlayer->SpawnID) return true;
		break;
	}

	if (!isTauntClassInGroup) return true;

	int myClass = pLocalPlayer->GetClass();
	switch (myClass) {
	case Shadowknight:
	case Warrior:
	case Paladin:
		if (isMainTank) return true;
		if (SpawnPctHPs(pLocalPlayer) < 50) return false;
		return true;
	case Bard:
	case Berserker:
	case Rogue:
	case Ranger:
	case Beastlord:
	case Monk:
		if (SpawnPctHPs(pLocalPlayer) < 50) return false;
		return true;
	case Cleric:
	case Druid:
	case Shaman:
	case Necromancer:
	case Enchanter:
	case Wizard:
	case Mage:
		return false;
	}
	return false;
}

// Is provided spell memorized?
bool IsSpellMemorized(PCHAR szName)
{
	
	string szSpellName = szName;
	PSPELL pSpell1 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell2 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell3 = GetSpellByName((char*)szSpellName.c_str());
	if (!pSpell1 && !pSpell2 && !pSpell3) {
		DebugSpewAlways("MQ2Elixir::IsSpellMemorized spell %s not found", szName);
		return false;
	}
	PSPELL pSpell = nullptr;

	for (int GEM = 0; GEM < NUM_SPELL_GEMS; GEM++) {
		if (pSpell1 && GetMemorizedSpell(GEM) == pSpell1->ID) pSpell = pSpell1;
		if (pSpell2 && GetMemorizedSpell(GEM) == pSpell2->ID) pSpell = pSpell2;
		if (pSpell3 && GetMemorizedSpell(GEM) == pSpell3->ID) pSpell = pSpell3;
		if (!pSpell) continue;
		return true;
	}
	return false;
}


// Check the cooldown of a spell
DWORD SpellCooldown(PCHAR szName)
{
	string szSpellName = szName;
	PSPELL pSpell1 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell2 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell3 = GetSpellByName((char*)szSpellName.c_str());
	if (!pSpell1 && !pSpell2 && !pSpell3) {
		DebugSpewAlways("MQ2Elixir::IsSpellReady spell %s not found", szName);
		return -1;
	}
	PSPELL pSpell = nullptr;

	for (int GEM = 0; GEM < NUM_SPELL_GEMS; GEM++) {
		if (pSpell1 && GetMemorizedSpell(GEM) == pSpell1->ID) pSpell = pSpell1;
		if (pSpell2 && GetMemorizedSpell(GEM) == pSpell2->ID) pSpell = pSpell2;
		if (pSpell3 && GetMemorizedSpell(GEM) == pSpell3->ID) pSpell = pSpell3;
		if (!pSpell) continue;
		return ((PSPAWNINFO)pLocalPlayer)->SpellGemETA[GEM];
	}
	return -1;
}

bool IsMoving(PSPAWNINFO pSpawn)
{
	if (!pSpawn) return false;
	if (pSpawn->SpeedRun != 0.0f) return true;
	if (fabs(FindSpeed(pSpawn)) != 0.0f) return true;
	return false;
}

bool ActionCast(PCHAR szName)
{
	if (ActionCastSpell(szName)) return true;
	if (ActionCastAbility(szName)) return true;
	if (ActionCastItem(szName)) return true;
	if (ActionCastAA(szName)) return true;
	if (ActionCastCombatAbility(szName)) return true;
	return false;
}

bool IsInSafeZone()
{
	return false;
}

bool AreObstructionWindowsVisible() 
{
	if (pSpellBookWnd->IsVisible()) return true;
	if (pGiveWnd->IsVisible()) return true;

	if (pBankWnd->IsVisible()) return true;
	if (pMerchantWnd->IsVisible()) return true;
	if (pTradeWnd->IsVisible()) return true;
	if (pLootWnd->IsVisible()) return true;
	return false;
}

// Does the spawn already have provided buff? 
// returns true on all failures, false exclusively when a buff candidate
bool HasBuff(PSPAWNINFO pSpawn, PCHAR szName)
{
	if (!pSpawn) {
		DebugSpewAlways("MQ2Elixir::HasBuff pSpawn nullptr");
		return true;
	}

	if (pSpawn->Type != SPAWN_PLAYER && pSpawn->Type != SPAWN_NPC) {
		DebugSpewAlways("MQ2Elixir::HasBuff pSpawn %s not player/npc", pSpawn->Name);
		return true;
	}

	if (!pLocalPC) {
		DebugSpewAlways("MQ2Elixir::HasBuff pLocalPC is nullptr");
		return true;
	}

	string szSpellName = szName;

	PSPELL pSpell1 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell2 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell3 = GetSpellByName((char*)szSpellName.c_str());
	if (!pSpell1 && !pSpell2 && !pSpell3) {
		DebugSpewAlways("MQ2Elixir::HasBuff could not find spell by name %s", szSpellName);
		return true;
	}

	// self buffs
	if (pSpawn->SpawnID == pLocalPlayer->SpawnID) {
        for (int i = 0; i < GetPcProfile()->GetMaxEffects(); i++) {
            EQ_Affect buff = GetPcProfile()->GetEffect(i);
            if (!buff.SpellID) continue;
            EQ_Spell* pBuffSpell = GetSpellByID(buff.SpellID);
            if (!pBuffSpell) continue;
            //if (pSpell1 && !WillStackWith(pSpell1, pBuffSpell)) return false;
            //if (pSpell2 && !WillStackWith(pSpell2, pBuffSpell)) return false;
            //if (pSpell3 && !WillStackWith(pSpell3, pBuffSpell)) return false;

			int slotIndex = -1;
			EQ_Affect* ret = pLocalPlayer->GetPcClient()->FindAffectSlot(pSpell1->ID, pLocalPlayer, &slotIndex, true, pLocalPlayer->Level);
			if (slotIndex != -1) return false;
			ret = pLocalPlayer->GetPcClient()->FindAffectSlot(pSpell2->ID, pLocalPlayer, &slotIndex, true, pLocalPlayer->Level);
			if (slotIndex != -1) return false;
			ret = pLocalPlayer->GetPcClient()->FindAffectSlot(pSpell3->ID, pLocalPlayer, &slotIndex, true, pLocalPlayer->Level);
			if (slotIndex != -1) return false;
        }
        return true;
    }

	// group buff inspect
	if (pLocalPC->Group) {
		CGroupMember* pG;
		PSPAWNINFO pGSpawn;
		for (int i = 0; i < 6; i++) {
			pG = pLocalPC->Group->GetGroupMember(i);
			if (!pG) continue;
			if (pG->Offline) continue;
			pGSpawn = pG->pSpawn;
			if (pGSpawn->SpawnID != pSpawn->SpawnID) continue;
			if (pGSpawn->Type == SPAWN_CORPSE) continue;
			for (int nBuff = 0; nBuff < NUM_BUFF_SLOTS; nBuff++)
			{
				int buffid = ((CTargetWnd*)pTargetWnd)->BuffSpellID[nBuff];
				if (buffid <= 0) {
					continue;
				}
				PSPELL pBuffSpell = GetSpellByID(buffid);
				if (!pBuffSpell) {
					continue;
				}
				if (pSpell1 && !WillStackWith(pSpell1, pBuffSpell)) return false;
				if (pSpell2 && !WillStackWith(pSpell2, pBuffSpell)) return false;
				if (pSpell3 && !WillStackWith(pSpell3, pBuffSpell)) return false;
				
				return true;
			}
		}
	}

	// target buff inspect
	if (pTarget && pSpawn->SpawnID == ((long)((PSPAWNINFO)pTarget)->SpawnID)) {
		// we are targetting the person we're checking buff on
		for (int nBuff = 0; nBuff < NUM_BUFF_SLOTS; nBuff++)
		{
			int buffid = ((CTargetWnd*)pTargetWnd)->BuffSpellID[nBuff];
			if (buffid <= 0) {
				continue;
			}
			PSPELL pBuffSpell = GetSpellByID(buffid);
			if (!pBuffSpell) {
				continue;
			}
			if (pSpell1 && !WillStackWith(pSpell1, pBuffSpell)) return false;
            if (pSpell2 && !WillStackWith(pSpell2, pBuffSpell)) return false;
            if (pSpell3 && !WillStackWith(pSpell3, pBuffSpell)) return false;
			//TODO: add level check comparison
			return true;
		}
	}
	return false;
}

bool IsNavActive()
{
	return NavActive();
}

MQPlugin* Plugin(char* plugin) 
{
	MQPlugin* pPlugin = pPlugins;
	while (pPlugins)
	{
		if (!_stricmp(plugin, pPlugins->szFilename)) return pPlugins;
		pPlugins = pPlugins->pNext;
	}
	return nullptr;
}

int CombatState() 
{
	if (!pPlayerWnd) return COMBATSTATE_NULL;
	DWORD state = ((CPlayerWnd*)pPlayerWnd)->CombatState;
	if (state == 0) {
		if ((CXWnd*)pPlayerWnd->GetChildItem("PW_CombatStateAnim")) return COMBATSTATE_COMBAT;
		return COMBATSTATE_NULL;
	}
	
	if (state > 4) return COMBATSTATE_UNKNOWN;
	return state;
}

bool IsFacingSpawn(PSPAWNINFO pSpawn)
{
	if (!pSpawn) {
		DebugSpewAlways("MQ2Elixir::IsFacingSpawn pSpawn nullptr");
		return false;
	}

	float fMyHeading = pLocalPlayer->Heading * 0.703125f;
	float fSpawnHeadingTo = (FLOAT)(atan2f(pLocalPlayer->Y - pSpawn->Y, pSpawn->X - pLocalPlayer->X) * 180.0f / PI + 90.0f);
	if (fSpawnHeadingTo < 0.0f)	fSpawnHeadingTo += 360.0f;
	else if (fSpawnHeadingTo >= 360.0f)	fSpawnHeadingTo -= 360.0f;
	float facing = abs(fSpawnHeadingTo - fMyHeading);
	return (facing <= 30);
}

int PctHPs() 
{
	if (!pLocalPlayer) return 0;
	if (pLocalPlayer->HPMax == 0) return 100;
	return (int)(pLocalPlayer->HPCurrent * 100 / pLocalPlayer->HPMax);
}

int SpawnPctHPs(PSPAWNINFO pSpawn)
{
	if (!pSpawn) return 100;
	if (pSpawn->HPMax < 1 || pSpawn->HPCurrent < 1) return 0;
	return (int)(pSpawn->HPCurrent * 100 / pSpawn->HPMax);
}

int SpawnPctEndurance(PSPAWNINFO pSpawn)
{
	if (!pSpawn) return 100;
	if (pSpawn->GetMaxEndurance() < 1 || pSpawn->GetCurrentEndurance() < 1) return 0;
	return (int)(pSpawn->GetCurrentEndurance() * 100 / pSpawn->GetMaxEndurance());
}

int PctMana() {	
	if (!pLocalPC) return 0;
	if (pLocalPC->Max_Mana() == 0) return 100;
	return (int)(pLocalPC->Cur_Mana() * 100 / pLocalPC->Max_Mana());
}

int SpawnPctMana(PSPAWNINFO pSpawn)
{
	if (!pSpawn) return 100;
	if (pSpawn->GetMaxMana() < 1 || pSpawn->GetCurrentMana() < 1) return 0;
	return (int)(pSpawn->GetCurrentMana() * 100 / pSpawn->GetMaxMana());
}

bool ActionSpawnTarget(PSPAWNINFO pSpawn)
{
	if (!pSpawn) {
		DebugSpewAlways("MQ2Elixir::ActionSpawnTarget pSpawn is nullptr");
		return false;
	}

	if (pTarget && pTarget->SpawnID == pSpawn->SpawnID) {
		DebugSpewAlways("MQ2Elixir::ActionSpawnTarget pSpawn %s already targetted", pSpawn->Name);
		return true;
	}
	pTarget = pSpawn;
	DebugSpewAlways("MQ2Elixir::ActionSpawnTarget pSpawn %s is now targetted", pSpawn->Name);
	return true;
}

bool ActionCastSpell(PCHAR szName)
{
	bool isBard = (pLocalPlayer->GetClass() == Bard);
	if (!isBard && IsMoving(pLocalPlayer)) {
		DebugSpewAlways("MQ2Elixir::ActionCastSpell is moving");
		return false;
	}
	if (pLocalPlayer->CastingData.IsCasting()) {
		DebugSpewAlways("MQ2Elixir::ActionCastSpell is casting");
		return false;
	}

	if (!IsCastingReady()) {
		DebugSpewAlways("MQ2Elixir::ActionCastSpell casting not ready");
		return false;
	}
	if (AreObstructionWindowsVisible()) {
		DebugSpewAlways("MQ2Elixir::ActionCastSpell obstruction window visible");
		return false;
	}

	string szSpellName = szName;
	PSPELL pSpell1 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell2 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell3 = GetSpellByName((char*)szSpellName.c_str());
	if (!pSpell1 && !pSpell2 && !pSpell3) {
		DebugSpewAlways("MQ2Elixir::ActionCastSpell spell %s not found", szName);
		return false;
	}

	PSPELL pSpell = nullptr;

	int gemIndex = -1;
	for (int GEM = 0; GEM < NUM_SPELL_GEMS; GEM++) {
		pSpell = nullptr;
		if (pSpell1 && GetMemorizedSpell(GEM) == pSpell1->ID) pSpell = pSpell1;
		if (pSpell2 && GetMemorizedSpell(GEM) == pSpell2->ID) pSpell = pSpell2;
		if (pSpell3 && GetMemorizedSpell(GEM) == pSpell3->ID) pSpell = pSpell3;
		if (!pSpell) continue;

		if (((CDisplay*)pDisplay)->TimeStamp <= ((PSPAWNINFO)pLocalPlayer)->SpellGemETA[GEM]) {
			DebugSpewAlways("MQ2Elixir::ActionCastSpell %s is on cooldown", szName);
			return false;
		}
		if ((int)((CDisplay*)pDisplay)->TimeStamp <= ((PSPAWNINFO)pLocalPlayer)->GetSpellCooldownETA()) {
			DebugSpewAlways("MQ2Elixir::ActionCastSpell %s waiting on global cooldown", szName);
			return false;
		}
		gemIndex = GEM;
		break;
	}
	if (gemIndex == -1) {
		DebugSpewAlways("MQ2Elixir::ActionCastSpell %s is not in gem list", szName);
		return false;
	}

	//BYTE    TargetType;         //03=Group v1, 04=PB AE, 05=Single, 06=Self, 08=Targeted AE, 0e=Pet, 28=AE PC v2, 29=Group v2, 2a=Directional
	if (pSpell->TargetType == 5 && !pTarget) {
		DebugSpewAlways("MQ2Elixir::ActionCastSpell %s targettype is single, no target provided", pSpell->Name);
		return false;
	}

	if (pSpell->SpellType == 0) {
		if (!pTarget) {
			DebugSpewAlways("MQ2Elixir::ActionCastSpell %s spelltype is detrimental, no target provided", pSpell->Name);
			return false;
		}

		if (pTarget->Type != SPAWN_NPC) {
			DebugSpewAlways("MQ2Elixir::ActionCastSpell %s spelltype is detrimental, target is not NPC", pSpell->Name);
			return false;
		}

	}
	//*0x180*/   BYTE    SpellType;          //0=detrimental, 1=Beneficial, 2=Beneficial, Group Only
	//if (spell->SpellType == 0) {
	if (!pSpell->CanCastInCombat && CombatState() == COMBATSTATE_COMBAT) {
		DebugSpewAlways("MQ2Elixir::ActionCastSpell spell %s cannot be cast in combat", pSpell->Name);
		return false;
	}


	if (pLocalPlayer->GetCurrentMana() < (int)pSpell->ManaCost) {
		DebugSpewAlways("MQ2Elixir::ActionCastSpell spell %s not enough mana", pSpell->Name);
		return false;
	}
	
	if (pTarget && Distance3DToSpawn(pLocalPlayer, (PSPAWNINFO)pTarget) > pSpell->Range) {
		DebugSpewAlways("MQ2Elixir::ActionCastSpell spell %s states target too far away", pSpell->Name);
		return false;
	}


	WriteChatf("[MQ2Elixir] %s %s%s", pSpell->Name, (pTarget&& pSpell->TargetType != 06) ? "-> " : "", (pTarget && pSpell->TargetType != 06) ? pTarget->Name : "");

	Execute("/cast %d", gemIndex + 1);
	return true;
}

bool ActionMemorizeSpell(WORD gem, PCHAR szName)
{
	if (gem < 1) {
		DebugSpewAlways("MQ2Elixir::ActionMemorizeSpell invalid gem %d < 1", gem);
		return false;
	}
	if (gem > NUM_SPELL_GEMS) {
		DebugSpewAlways("MQ2Elixir::ActionMemorizeSpell invalid gem %d > %d", gem, NUM_SPELL_GEMS);
		return false;
	}

	string szSpellName = szName;
	PSPELL pSpell1 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell2 = GetSpellByName((char*)szSpellName.c_str());
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell3 = GetSpellByName((char*)szSpellName.c_str());
	if (!pSpell1 && !pSpell2 && !pSpell3) {
		DebugSpewAlways("MQ2Elixir::ActionMemorizeSpell spell %s not found", szName);
		return false;
	}

	PSPELL pSpell = NULL;
	PSPELL pSpellInBook;

	for (DWORD nSpell = 0; nSpell < NUM_BOOK_SLOTS; nSpell++) {
		pSpellInBook = GetSpellByID(GetPcProfile()->SpellBook[nSpell]);
		if (!pSpellInBook) continue;

		if (pSpell1 && pSpellInBook->ID == pSpell1->ID) pSpell = pSpell1;
		if (pSpell2 && pSpellInBook->ID == pSpell2->ID) pSpell = pSpell2;
		if (pSpell3 && pSpellInBook->ID == pSpell3->ID) pSpell = pSpell3;
		if (!pSpell) continue;
		break;
	}
	if (!pSpell) {
		DebugSpewAlways("MQ2Elixir::ActionMemorizeSpell spell %s not found in spellbook", pSpell->Name);
		return false;
	}


	if (pSpell->ClassLevel[pLocalPlayer->mActorClient.Class] > pLocalPlayer->Level) {
		DebugSpewAlways("MQ2Elixir::ActionMemorizeSpell spell %s needs at least level %d, got level %d", pSpell->Name, pSpell->ClassLevel[pLocalPlayer->mActorClient.Class], pLocalPlayer->Level);
		return false;
	}

	SPELLFAVORITE MemSpellFavorite = SPELLFAVORITE();
	DWORD Favorite = (DWORD)&MemSpellFavorite;
	
	ZeroMemory(&MemSpellFavorite, sizeof(MemSpellFavorite));
	strcpy_s(MemSpellFavorite.Name, "Mem a Spell");
	MemSpellFavorite.inuse = 1;
	for (auto sp = 0; sp < NUM_SPELL_GEMS; sp++) {
		MemSpellFavorite.SpellId[sp] = 0xFFFFFFFF;
	}
	MemSpellFavorite.SpellId[gem] = pSpell->ID;
	pSpellBookWnd->MemorizeSet((int*)Favorite, NUM_SPELL_GEMS);
	return true;
}

void Execute(PCHAR zFormat, ...)
{
	char zOutput[MAX_STRING] = { 0 }; va_list vaList; va_start(vaList, zFormat);
	vsprintf_s(zOutput, zFormat, vaList);
	if (!zOutput[0]) {
		return;
	}
	DoCommand(pLocalPlayer, zOutput);
}

bool ActionCastItem(PCHAR szName)
{
	if (!IsCastingReady()) return false;
	auto pItem = FindItemByName(szName, 1);
	if (!pItem) return false;

	Execute("/useitem \"%s\"", szName);	
	return true;
}

bool ActionCastAA(PCHAR szName)
{
	bool isBard = (pLocalPlayer->GetClass() == Bard);
	if (!isBard && !IsCastingReady()) {
		DebugSpewAlways("MQ2Elixir::ActionCastAA not bard, casting isn't ready for %s", szName);
		return false;
	}

	int level = -1;
	if (PSPAWNINFO pMe = (PSPAWNINFO)pLocalPlayer) {
		level = pMe->Level;
	}

	for (unsigned long nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
		PALTABILITY pAbility = GetAAByIdWrapper(pPCData->GetAlternateAbilityId(nAbility), level);
		if (!pAbility) continue;
		
		PCHAR pName = pCDBStr->GetString(pAbility->nName, 1, nullptr);
		if (!pName) continue;
		if (_stricmp(pName = pCDBStr->GetString(pAbility->nName, 1, nullptr), szName)) continue;

		if (!pAltAdvManager->IsAbilityReady(pPCData, pAbility, 0)) {
			DebugSpewAlways("MQ2Elixir::ActionCastAA ability %s not ready", szName);
			return false;
		}
		DebugSpewAlways("MQ2Elixir::ActionCastAA activating %s", szName);
		Execute("/alt act %d", pAbility->ID);
		return true;
	}
	DebugSpewAlways("MQ2Elixir::ActionCastAA could not find %s", szName);
	return false;
}

bool IsAbilityReady(PCHAR szName)
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		DWORD nToken = pSkillMgr->GetNameToken(i);
		if (!HasSkill(i)) continue;
		const char* abilityName = pStringTable->getString(nToken, 0);
		if (!abilityName) continue;	
		if (!pSkillMgr->IsActivatedSkill(i)) continue;

		string szSpellName = szName;
		if (!_stricmp(szSpellName.c_str(), abilityName)) {
			DebugSpewAlways("MQ2Elixir::IsAbilityReady ability %s available check", szSpellName);
			return pSkillMgr->IsAvailable(i);
		}
		szSpellName = szName;
		szSpellName.append(" Rk. II");
		if (!_stricmp(szSpellName.c_str(), abilityName)) {
			DebugSpewAlways("MQ2Elixir::IsAbilityReady ability %s available check", szSpellName);
			return pSkillMgr->IsAvailable(i);
		}
		szSpellName = szName;
		szSpellName.append(" Rk. II");
		if (!_stricmp(szSpellName.c_str(), abilityName)) {
			DebugSpewAlways("MQ2Elixir::IsAbilityReady ability %s available check", szSpellName);
			return pSkillMgr->IsAvailable(i);
		}
	}
	DebugSpewAlways("MQ2Elixir::IsAbilityReady ability %s not found", szName);
	return false;
}

bool ActionCastAbility(PCHAR szName)
{
	if (!IsCastingReady()) {
		DebugSpewAlways("MQ2Elixir::ActionCastAbility casting not ready");
		return false;
	}

	for (int i = 0; i < NUM_SKILLS; i++) {
		DWORD nToken = pSkillMgr->GetNameToken(i);
		if (!HasSkill(i)) continue;
		const char* abilityName = pStringTable->getString(nToken, 0);
		if (!abilityName) continue;
		if (!pSkillMgr->IsActivatedSkill(i)) continue;

		string szSpellName = szName;
		if (!_stricmp(szSpellName.c_str(), abilityName) && pSkillMgr->IsAvailable(i)) {
			char temp[MAX_STRING] = "";
			sprintf_s(temp, "\"%s", szSpellName);
			strcat_s(temp, MAX_STRING, "\"");
			DebugSpewAlways("MQ2Elixir::ActionCastAbility doability %s", szSpellName);
			DoAbility(pLocalPlayer, temp);
			return true;
		}
		szSpellName = szName;
		szSpellName.append(" Rk. II");
		if (!_stricmp(szSpellName.c_str(), abilityName) && pSkillMgr->IsAvailable(i)) {
			char temp[MAX_STRING] = "";
			sprintf_s(temp, "\"%s", szSpellName);
			strcat_s(temp, MAX_STRING, "\"");
			DebugSpewAlways("MQ2Elixir::ActionCastAbility doability %s", szSpellName);
			DoAbility(pLocalPlayer, temp);
			return true;
		}
		szSpellName = szName;
		szSpellName.append(" Rk. II");
		if (!_stricmp(szSpellName.c_str(), abilityName) && pSkillMgr->IsAvailable(i)) {
			char temp[MAX_STRING] = "";
			sprintf_s(temp, "\"%s", szSpellName);
			strcat_s(temp, MAX_STRING, "\"");
			DebugSpewAlways("MQ2Elixir::ActionCastAbility doability %s", szSpellName);
			DoAbility(pLocalPlayer, temp);
			return true;
		}
	}
	DebugSpewAlways("MQ2Elixir::ActionCastAbility ability %s not found", szName);
	return false;
}


bool IsCombatAbilityReady(PCHAR szName)
{
	if (!IsCastingReady()) {
		DebugSpewAlways("MQ2Elixir::IsCombatAbilityReady casting not ready");
		return false;
	}

	PSPELL pSpell = nullptr;
	string szSpellName = szName;
	PSPELL pSpell1 = GetSpellByName((char*)szSpellName.c_str());
	if (pSpell1) pSpell = pSpell1;
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell2 = GetSpellByName((char*)szSpellName.c_str());
	if (pSpell2) pSpell = pSpell2;
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell3 = GetSpellByName((char*)szSpellName.c_str());
	if (pSpell3) pSpell = pSpell3;
	if (!pSpell) {
		DebugSpewAlways("MQ2Elixir::IsCombatAbilityReady spell %s not found", szName);
		return false;
	}

	DWORD timeNow = (DWORD)time(nullptr);
#if !defined(ROF2EMU) && !defined(UFEMU)
	if (pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex, pSpell->SpellGroup) > timeNow) {
		return false;
	}
#else
	if (pSpell->ReuseTimerIndex == -1 || pSpell->ReuseTimerIndex > 20) {
		return false;
	}
	if (pPCData->GetCombatAbilityTimer(pSpell->ReuseTimerIndex) > timeNow) {
		return false;
	}
#endif
	if (HasBuff(pLocalPlayer, pSpell->Name)) {
		return false;
	}

	if (strstr(pSpell->Name, "Discipline") && pCombatAbilityWnd) {
		CXWnd* Child = ((CXWnd*)pCombatAbilityWnd)->GetChildItem("CAW_CombatEffectLabel");
		if (!Child) {
			DebugSpewAlways("MQ2Elixir::IsCombatAbilityReady window effect label not found");
			return false;
		}
		CHAR szBuffer[2048] = { 0 };
		GetCXStr(Child->CGetWindowText(), szBuffer, MAX_STRING);
		if (szBuffer[0] == '\0') {
			DebugSpewAlways("MQ2Elixir::IsCombatAbilityReady window text is not null terminated");
			return false;
		}
		PSPELL pBuff = GetSpellByName(szBuffer);
		if (pBuff) {
			DebugSpewAlways("MQ2Elixir::IsCombatAbilityReady active disc is already %s, can't use %s", pBuff->Name, pSpell->Name);
			return false;
		}
	}

	if (pLocalPlayer->GetCurrentMana() < (int)pSpell->ManaCost) {
		DebugSpewAlways("MQ2Elixir::IsCombatAbilityReady not enough mana for %s", pSpell->Name);
		return false;
	}

	if (pLocalPlayer->GetCurrentEndurance() < (int)pSpell->EnduranceCost) {
		DebugSpewAlways("MQ2Elixir::IsCombatAbilityReady not enough mana for %s", pSpell->Name);
		return false;
	}


	if (pTarget && Distance3DToSpawn(pLocalPlayer, (PSPAWNINFO)pTarget) > pSpell->Range) {
		DebugSpewAlways("MQ2Elixir::IsCombatAbilityReady target too far away for %s", pSpell->Name);
		return false;
	}

	DWORD ReqID = pSpell->CasterRequirementID;
	if (ReqID == 518 && SpawnPctHPs(pLocalPlayer) > 89) return false;
	if (ReqID == 825 && SpawnPctEndurance(pLocalPlayer) > 20) return false;
	if (ReqID == 826 && SpawnPctEndurance(pLocalPlayer) > 24) return false;
	if (ReqID == 827 && SpawnPctEndurance(pLocalPlayer) > 29) return false;

	for (int i = 0; i < 4; i++) {
		if (pSpell->ReagentCount[i] == 0) continue;
		if (pSpell->ReagentID[i] == -1) continue;

		DWORD count = FindItemCountByID((int)pSpell->ReagentID[i]);
		if (count > pSpell->ReagentCount[i]) continue;

		PCHAR pItemName = GetItemFromContents(FindItemByID((int)pSpell->ReagentID[i]))->Name;
		if (pItemName) {
			DebugSpewAlways("MQ2Elixir::IsCombatAbilityReady %s needs %ix %s", pSpell->Name, pSpell->ReagentCount[i], pItemName);
			return false;
		}						
	}
	return true;
}


bool ActionCastCombatAbility(PCHAR szName)
{

	PSPELL pSpell = nullptr;
	string szSpellName = szName;
	PSPELL pSpell1 = GetSpellByName((char*)szSpellName.c_str());
	if (pSpell1) pSpell = pSpell1;
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell2 = GetSpellByName((char*)szSpellName.c_str());
	if (pSpell2) pSpell = pSpell2;
	szSpellName = szName;
	szSpellName.append(" Rk. II");
	PSPELL pSpell3 = GetSpellByName((char*)szSpellName.c_str());
	if (pSpell3) pSpell = pSpell3;
	if (!pSpell) {
		DebugSpewAlways("MQ2Elixir::IsCombatAbilityReady spell %s not found", szName);
		return false;
	}

	return (pLocalPC->DoCombatAbility(pSpell->ID));
}


PALTABILITY AAByName(PCHAR Name) {
	int level = -1;
	/*if (PSPAWNINFO pMe = (PSPAWNINFO)pLocalPlayer) {
		level = pMe->Level;
	}*/
	for (unsigned long nAbility = 0; nAbility < NUM_ALT_ABILITIES; nAbility++) {
		if (PALTABILITY pAbility = GetAAByIdWrapper(nAbility, level)) {
			if (char* pName = pCDBStr->GetString(pAbility->nName, 1, NULL)) {
				if (!_stricmp(Name, pName))
					return pAbility;
			}
		}
	}
	return NULL;
}

PSPAWNINFO PetTarget()
{
	long petID = pLocalPlayer->PetID;
	if (petID <= 0) return nullptr;
	if (petID == 0xFFFFFFFF) return nullptr;

	EQPlayer* pPet = GetSpawnByID(petID);
	if (!pPet) return nullptr;
	return pPet->WhoFollowing;
}

/*
bool SpellStacks() 
{
	if (GetSpellDuration(buffSpell, (PSPAWNINFO)pLocalPlayer) >= 0xFFFFFFFE) {
		buffduration = 99999 + 1;
	}
	if (!BuffStackTest(pSpell, buffSpell, TRUE) || ((buffSpell == pSpell) && (buffduration > duration))) {
		//Dest.DWord = false;
		//return true;
		return false;
	}
}
*/

bool ActionCastGem(int gemIndex) 
{
#if defined(ROF2EMU) || defined(UFEMU)
	if (pLocalPlayer->StandState == STANDSTATE_SIT) {
		DoCommandf("/stand");
	}
#endif
	Execute("/cast %d", gemIndex);
	return true;
}

PCONTENTS EquippedSlot(PCONTENTS pCont)
{
	if (PITEMINFO pItem = GetItemFromContents(pCont))
	{
		DWORD cmp = pItem->EquipSlots;
		for (int N = 0; N < 32; N++)
		{
			if (cmp & (1 << N))
			{
				if (PCONTENTS pInvSlot =pLocalPC->GetCurrentPcProfile()->pInventoryArray->InventoryArray[N])
				{
					return pInvSlot;
				}
			}
		}
	}
	return NULL;
}


bool IsAAPurchased(PCHAR AAName) {
	DWORD level = -1;
	if (PSPAWNINFO pMe = (PSPAWNINFO)pLocalPlayer) {
		level = pMe->Level;
	}

	for (unsigned long nAbility = 0; nAbility < AA_CHAR_MAX_REAL; nAbility++) {
		PALTABILITY pAbility = GetAAByIdWrapper(pPCData->GetAlternateAbilityId(nAbility), level);

		//test for good structure, and level as quick fail fast, the above wrapper not use level on EMU
		if (pAbility && pAbility->MinLevel <= level) {
			PCHAR pName = pCDBStr->GetString(pAbility->nName, 1, NULL);
			if (pName && !_stricmp(AAName, pName)) {
				// good level,  good name,   return postive find.
				return true;
			}
		}
	}
	// failed to find by level and name checks,  return negative find.
	return false;
}


bool IsSpellStackable(PSPAWNINFO pSpawn, PSPELL pSpell) {
	if (!pSpell) return false;
	if (!pSpawn) return false;

	if (pTarget && pSpawn->SpawnID == ((long)((PSPAWNINFO)pTarget)->SpawnID)) {
		// we are targetting the person we're checking buff on
		for (int nBuff = 0; nBuff < NUM_BUFF_SLOTS; nBuff++)
		{
			int buffid = ((CTargetWnd*)pTargetWnd)->BuffSpellID[nBuff];
			if (buffid <= 0) continue;
			PSPELL pBuffSpell = GetSpellByID(buffid);
			if (!pBuffSpell) continue;
			if (!IsSpellStackableCompare(pSpell, pBuffSpell)) return false;
		}
	}
	else if (pSpawn->SpawnID == pLocalPlayer->SpawnID) {
		for (int i = 0; i < NUM_LONG_BUFFS; i++) {
			
			PSPELL pBuffSpell = GetSpellByID(GetPcProfile()->Buffs[i].SpellID);
			if (!pBuffSpell) continue;
			if (!IsSpellStackableCompare(pSpell, pBuffSpell)) return false;
		}
	}
	return true;
}

bool IsSpellStackableCompare(PSPELL pSpell1, PSPELL pSpell2) {
	if (pSpell1->ID == pSpell2->ID) return false; //assume if same spell, that it's not worth recasting
	if (pSpell1->ID == 2751) return false; //manaburn check
	if (IsSPAEffect(pSpell1, 350)) return false; //mana burn SE
	int64_t attr1;
	int64_t base11;
	int64_t base12;
	int64_t max1;
	int64_t calc1;
	int64_t attr2;
	int64_t base21;
	int64_t base22;
	int64_t max2;
	int64_t calc2;

	for (int i = 0; i < GetSpellNumEffects(pSpell1); i++) {
		attr1 = GetSpellAttrib(pSpell1, i);
		base11 = GetSpellBase(pSpell1, i);
		base12 = GetSpellBase2(pSpell1, i);
		max1 = GetSpellMax(pSpell1, i);
		calc1 = GetSpellCalc(pSpell1, i);
		attr2 = GetSpellAttrib(pSpell2, i);
		base21 = GetSpellBase(pSpell2, i);
		base22 = GetSpellBase2(pSpell2, i);
		max2 = GetSpellMax(pSpell2, i);
		calc2 = GetSpellCalc(pSpell2, i);
		if (attr1 == attr1) return false;
	}
	return true;
}


unsigned long StunDuration(PSPELL pSpell) {
	int64_t attr;
	//int64_t base;
	int64_t base2;
	//int64_t max;
	//int64_t calc;

	for (int i = 0; i < GetSpellNumEffects(pSpell); i++) {
		attr = GetSpellAttrib(pSpell, i);
		//base = GetSpellBase(pSpell, i);
		base2 = GetSpellBase2(pSpell, i);
		//max = GetSpellMax(pSpell, i);
		//calc = GetSpellCalc(pSpell, i);
		if (attr == 21) return base2;
	}
	return 0;
}

unsigned long MainAssistTargetID() {
	if (pLocalPC->pGroupInfo) return 0;
	return ((PSPAWNINFO)pLocalPlayer)->GroupAssistNPC[0];
}

unsigned int PetTargetID() {
	int petID = pLocalPlayer->PetID;
	if (!petID) return petID;
	if (petID == 0xFFFFFFFF) return 0;
	EQPlayer* pPet = GetSpawnByID(petID);
	if (!pPet) return 0;
	if (!pPet->WhoFollowing) return 0;
	return pPet->WhoFollowing->SpawnID;
}