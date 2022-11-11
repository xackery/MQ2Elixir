#include <mq/Plugin.h>
#include "Core.h"
#include "Elixir.h"
#include "Mob.h"

using namespace std;

// Spell returns a string with a reason if provided spell cannot be cast
std::string  Elixir::Spell(PSPELL pSpell) {
	CHAR szTemp[MAX_STRING] = { 0 };
		
	PCHARINFO pChar = (PCHARINFO)pCharData;
	if (!pChar) return "char not loaded";
	if (!pLocalPlayer) return "pLocalPlayer not loaded";

	bool isHeal = false;
	bool isDebuff = false;
	bool isBuff = false;
	bool isLifetap = false;
	bool isMana = false;
	bool isCharm = false;
	bool isSnare = false;
	bool isSow = false;	
	bool isTaunt = false;
	bool isSingleTargetSpell = false;
	bool isPetSummon = false;
	bool isTransport = false;
	bool isGroupSpell = false;
	bool isBardSong = false;
	bool isMez = false;
	bool isLull = false;
	int64_t stunDuration = 0;
	int64_t damageAmount = 0;
	int64_t healAmount = 0;

	int bodyType;
	int64_t attr;
	int64_t base;
	int64_t base2;
	int64_t max;
	int64_t calc;
	int64_t spellgroup = pSpell->SpellGroup;
	int64_t ticks = pSpell->DurationCap;
	int64_t targets = pSpell->MaxTargets;
	int64_t targettype = pSpell->TargetType;
	int64_t skill = pSpell->Skill;
	PSPELL pSpellRecourse = nullptr;
	if (pSpell->Autocast > 0) {
		pSpellRecourse = GetSpellByID(pSpell->Autocast);
	}


	for (int i = 0; i < GetSpellNumEffects(pSpell); i++) {
		attr = GetSpellAttrib(pSpell, i);
		base = GetSpellBase(pSpell, i);
		base2 = GetSpellBase2(pSpell, i);
		max = GetSpellMax(pSpell, i);
		calc = GetSpellCalc(pSpell, i);

		if (attr == SPA_HP) { //0
			if (max > 0) { //Heal / HoT
				if (ticks < 5 && base > 0) { //regen etc are not heals
					isHeal = true;
					healAmount = base;
				}
				if (ticks > 0) {
					isBuff = true;
				}
				if (pSpell->Category == 114) { //Taps like touch of zlandicar
					isLifetap = true;
				}
			}
			if (base < 0 && damageAmount == 0) {
				damageAmount = -base;
			}
			if (max < 0) { //Nuke / DoT
				damageAmount = -max;
			}
		}
		if (attr == SPA_AC || //1
			attr == 2 || //attack
			attr == 4 || //str
			attr == 5 || //dex
			attr == 6 || //agi
			attr == 7 || //sta
			attr == 8 || //int
			attr == 9 //wis
			) {
			if (base > 0) { //+stat
				isBuff = true;
			}
			if (base < 0) { //-stat
				isDebuff = true;
			}
		}

		if (attr == SPA_MOVEMENT_RATE) { //3
			if (base > 0) { //+Movement
				isBuff = true;
				isSow = true;
			}
			if (base < 0) { //-Movement
				isDebuff = true;
				isSnare = true;
			}
		}
		
		if (attr == SPA_CHA) { //10
			if (base > 0 && base < 254) { //+CHA
				isBuff = true;
			}
			if (base < 0) { //-CHA
				isDebuff = true;
			}
		}
		if (attr == SPA_HASTE) {
			if (base > 0) { //+Haste
				isBuff = true;
			}
			if (base < 0) { //-Haste
				isDebuff = true;
			}
		}

		if (attr == 15) { // Mana
			isMana = true;
		}
		if (attr == 18) { // pacify (lull)
			isLull = true;
		}
		if (attr == 21) { // stun
			stunDuration = base2;
			if (pSpell->TargetType == 2 || pSpell->TargetType == 4) {
				isSingleTargetSpell = true; //hack to make ae stuns work
			}
		}
		if (attr == 22) { // charm
			isCharm = true;
		}

		if (attr == 26) { // Gate
			isTransport = true;
		}
		if (attr == 30) { // frenzy radius reduction (lull)
			isLull = true;
		}
		if (attr == 86) { // reaction radius reduction (lull)
			isLull = true;
		}

		if (attr == 31) { // Mesmerization
			isMez = true;
		}

		if (attr == 33) { // Summon Elemental Pet
			isPetSummon = true;
		}


		if (attr == 71) { // Summon Skeleton Pet
			isPetSummon = true;
		}
		if (attr == 83) { // Transport
			isTransport = true;
		}
		if (attr == 88) { // Evac
			isTransport = true;
		}

		if (attr == 108) { //Summon Familiar
			isPetSummon = true;
		}

		if (attr == 192) { //taunt
			isTaunt = true;
		}
	}

	if (pSpell->Subcategory == 43 && ticks < 10) { //Health
		isHeal = true;
	}

	if (pSpell->Category == 126) { //Taps
		if (pSpell->Subcategory == 43) { //Health
			isLifetap = true;
		}
	}

	/*
	//TODO TargetTypes:
	case 40: return "AE PC v2";
	case 25: return "AE Summoned";
	case 24: return "AE Undead";
	case 20: return "Targetted AE Tap";
	case  8: return "Targetted AE";
	case  2: return "AE PC v1";
	case  1: return "Line of Sight";
	*/

	if (pSpell->TargetType == 41) { // Group v2
		isGroupSpell = true;
	}
	if (pSpell->TargetType == 3) { // Group v1
		isGroupSpell = true;
	}

	if (isGroupSpell && pSpell->CastTime == 3000) {
		isBardSong = true;
	}

	if (pSpell->TargetType == 5) { //Single
		isSingleTargetSpell = true;
	}

	if (pTarget) {
		bodyType = GetBodyType((PSPAWNINFO)pTarget);
	}

	if (pSpell->TargetType == 16) { //Animal
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 10) { //Undead
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 11) { //Summoned
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 13) { //Lifetap
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 14) { //Pet
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 15) { //Corpse
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 16) { //Plant
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 17) { //Uber Giants
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 18) { //Uber Dragons
		isSingleTargetSpell = true;
	}

	if (IsDebugTagMode) {
		sprintf_s(szTemp, "mana %d", pSpell->ManaCost);
		if (isHeal) sprintf_s(szTemp, "%s %s", szTemp, "isHeal");
		if (isDebuff) sprintf_s(szTemp, "%s %s", szTemp, "isDebuff");
		if (isBuff) sprintf_s(szTemp, "%s %s", szTemp, "isBuff");
		if (isLifetap) sprintf_s(szTemp, "%s %s", szTemp, "isLifetap");
		if (isMana) sprintf_s(szTemp, "%s %s", szTemp, "isMana");
		if (isSnare) sprintf_s(szTemp, "%s %s", szTemp, "isSnare");
		if (isSow) sprintf_s(szTemp, "%s %s", szTemp, "isSow");
		if (isTaunt) sprintf_s(szTemp, "%s %s hateAmount %d", szTemp, "isTaunt", pSpell->HateGenerated);
		if (isSingleTargetSpell) sprintf_s(szTemp, "%s %s", szTemp, "isSingleTargetSpell");
		if (isPetSummon) sprintf_s(szTemp, "%s %s", szTemp, "isPetSummon");
		if (isTransport) sprintf_s(szTemp, "%s %s", szTemp, "isTransport");
		if (isGroupSpell) sprintf_s(szTemp, "%s %s", szTemp, "isGroupSpell");
		if (isBardSong) sprintf_s(szTemp, "%s %s", szTemp, "isBardSong");
		if (isCharm) sprintf_s(szTemp, "%s %s", szTemp, "isCharm");
		if (isMez) sprintf_s(szTemp, "%s %s", szTemp, "isMez");
		if (isLull) sprintf_s(szTemp, "%s %s", szTemp, "isLull");
		if (isMez) sprintf_s(szTemp, "%s %s", szTemp, "isMez");
		if (stunDuration) sprintf_s(szTemp, "%s %s %lldms", szTemp, "stunDuration", stunDuration);
		if (damageAmount) sprintf_s(szTemp, "%s %s %lld", szTemp, "damageAmount", damageAmount);
		if (healAmount) sprintf_s(szTemp, "%s %s %lld", szTemp, "healAmount", healAmount);
		return szTemp;
	}


	if (GetPcProfile()->Mana < (int)pSpell->ManaCost) return "not enough mana (" + to_string((int)pSpell->ManaCost) + "/" + to_string(GetPcProfile()->Mana) + ")";

	if (isLull) return "ignored (lull)";
	if (isMez) return "ignored (mez)";
	if (isCharm) return "ignored (charm)";

	if (pSpell->TargetType == 16) { //Animal
		if (!pTarget) return "no animal target";
		if (bodyType != 21) return "target not animal";
	}

	if (pSpell->TargetType == 10) { //Undead
		if (!pTarget) return "no undead target";
		if (bodyType != 3) return "target not undead";
	}

	if (pSpell->TargetType == 11) { //Summoned
		if (!pTarget) return "no summoned target";
		if (bodyType != 28) return "target not summoned";
	}

	if (pSpell->TargetType == 16) { //Plant
		if (!pTarget) return "no plant target";
		if (bodyType != 25) return "target not plant";
	}
	
	if (isTransport) return "ignoring (transport)";

	if (pSpell->NoNPCLOS == 0 && pTarget && isSingleTargetSpell && !pCharSpawn->CanSee(*pTarget)) return "target not line of sight";
	
	for (int i = 0; i < 4; i++) { // Reagent check
		if (pSpell->ReagentCount[i] == 0) continue;
		if (pSpell->ReagentID[i] == -1) continue;

		int count = FindItemCountByID((int)pSpell->ReagentID[i]);
		if (count > pSpell->ReagentCount[i]) continue;

		PCHAR pItemName = GetItemFromContents(FindItemByID((int)pSpell->ReagentID[i]))->Name;
		if (pItemName) {
			sprintf_s(szTemp, "missing %dx %s", pSpell->ReagentCount[i], pItemName);
			return szTemp;
		}
	}


	int ReqID = pSpell->CasterRequirementID;
	if (ReqID == 518 && SpawnPctHPs(pChar->pSpawn) > 89) return "not < 90% hp";

	//if (ReqID == 825 && SpawnPctEndurance(pChar->pSpawn) > 20) return false;
	//if (ReqID == 826 && SpawnPctEndurance(pChar->pSpawn) > 24) return false;
	//if (ReqID == 827 && SpawnPctEndurance(pChar->pSpawn) > 29) return false;

	if ((isHeal || isLifetap) && !IsHealAIRunning) return "heal AI not running";

	if (pSpellRecourse && pSpellRecourse->DurationCap > 0) { //recourse buff attached
		for (unsigned long nBuff = 0; nBuff < NUM_LONG_BUFFS; nBuff++) {
			if (pSpellRecourse->TargetType == 6) { //self buff
				PSPELL buff = GetSpellByID(GetPcProfile()->Buffs[nBuff].SpellID);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpellRecourse->ID) return "recourse already have buff";
				if (!BuffStackTest(buff, pSpellRecourse)) return "recourse does not stack";
			}

			int buffid = ((CTargetWnd*)pTargetWnd)->BuffSpellID[nBuff];
			if (buffid <= 0) {
				continue;
			}
			PSPELL pBuffSpell = GetSpellByID(buffid);
			if (!pBuffSpell) {
				continue;
			}

			DWORD buffDuration = ((CTargetWnd*)pTargetWnd)->BuffTimer[nBuff];
			if (buffDuration <= 0) {
				continue;
			}
			if (!BuffStackTest(pBuffSpell, pSpellRecourse)) {
				return "debuff already on";
			}
			if (pSpellRecourse->TargetType == 8) { //targetted
				if (pBuffSpell->ID == pSpellRecourse->ID) return "recourse already on target";
				if (!BuffStackTest(pBuffSpell, pSpellRecourse)) return "recourse does not stack on target";
			}
		}
	}


	if (ticks > 0 && pSpell->SpellType == 0) { // debuff
		for (unsigned long nBuff = 0; nBuff < NUM_LONG_BUFFS; nBuff++) {
			
			int buffid = ((CTargetWnd*)pTargetWnd)->BuffSpellID[nBuff];
			if (buffid <= 0) {
				continue;
			}
			PSPELL pBuffSpell = GetSpellByID(buffid);
			if (!pBuffSpell) {
				continue;
			}

			DWORD buffDuration = ((CTargetWnd*)pTargetWnd)->BuffTimer[nBuff];
			if (buffDuration <= 0) {
				continue;
			}
			if (!BuffStackTest(pBuffSpell, pSpell)) return "debuff already on";

			if (pBuffSpell->ID == pSpell->ID) return "debuff already on target";
			if (!BuffStackTest(pBuffSpell, pSpell)) return "debuff does not stack on target";
		}
	}

	PZONEINFO pZone = reinterpret_cast<PZONEINFO>(pZoneInfo);
	if (pZone && pSpell->ZoneType == 1) { //can't cast indoors
		if (pZone->ZoneType == EOutDoor::IndoorDungeon) return "in dungeon";
		if (pZone->ZoneType == EOutDoor::IndoorCity) return "in city";
		if (pZone->ZoneType == EOutDoor::DungeonCity) return "in dungeon city";
	}

	if (pZone && pSpell->ZoneType == 2) { //can't cast outdoor
		if (pZone->ZoneType == EOutDoor::Outdoor) return "outdoors";
		if (pZone->ZoneType == EOutDoor::OutdoorCity) return "outdoor city";
		if (pZone->ZoneType == EOutDoor::OutdoorDungeon) return "outdoor dungeon";
	}


	if (!pSpell->CanCastInCombat && CombatState() == COMBATSTATE_COMBAT) return "cannot be cast in combat";
	
	
	if (GetPcProfile()->Endurance < (int)pSpell->EnduranceCost) return "not enough endurance (" + to_string((int)pSpell->EnduranceCost) + "/" + to_string(GetPcProfile()->Endurance) + ")";	

	if (strstr(pSpell->Name, "Discipline") && pCombatAbilityWnd) {
		CXWnd* Child = ((CXWnd*)pCombatAbilityWnd)->GetChildItem("CAW_CombatEffectLabel");
		if (!Child) return "combatabilitywnd not found";

		CHAR szBuffer[2048] = { 0 };
		GetCXStr(Child->CGetWindowText(), szBuffer, MAX_STRING);
		if (szBuffer[0] == '\0') return "combatabilitywnd text not null terminated";
		PSPELL pBuff = GetSpellByName(szBuffer);
		if (pBuff) {
			sprintf_s(szTemp, "disc %s already active", pBuff->Name);
			return szTemp;
		}
	}

	if (isPetSummon && pChar->pSpawn->PetID > 0) return "already have pet";

	if (isPetSummon) {
		for (unsigned long nBuff = 0; nBuff < NUM_LONG_BUFFS; nBuff++) {
			if (GetPcProfile()->Buffs[nBuff].SpellID == pSpell->ID) return "already have pet buff";
		}
	}


	if (isMana && pSpell->TargetType == 6 && ticks <= 0 && !isPetSummon) { // self only mana regen, like harvest, canni
		if (stunDuration > 0 && CombatState() == COMBATSTATE_COMBAT) return "in combat, has stun attached";

		if (SpawnPctMana(pChar->pSpawn) > 50) return "not < 50 % mana";
		return "";
	}


	if (isLifetap && SpawnPctHPs(pChar->pSpawn) > HealAIMax) {
		sprintf_s(szTemp, "> %d%% hp", HealAIMax);
		return szTemp;
	}

	if (pSpell->SpellType >= 1 && (pSpell->TargetType == 6 || isGroupSpell)) { //self/group beneficial spell
		//if (pSpell->CastTime > 1000 && CombatState() == COMBATSTATE_COMBAT && !isBardSong) {
		//	return "cast time > 1s, long for combat";
		//}
		if (CombatState() == COMBATSTATE_COMBAT && !isBardSong) return "cast time > 1s, long for combat";

		
		if (ticks > 0) {
			if (!IsBuffAIRunning) return "buff ai not running";

			for (int b = 0; b < NUM_LONG_BUFFS; b++)
			{
				PSPELL buff = GetSpellByID(GetPcProfile()->Buffs[b].SpellID);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpell->ID) return "already have buff";
				if (!BuffStackTest(buff, pSpell)) return "does not stack";
				if (pSpellRecourse) {
					if (buff->ID == pSpellRecourse->ID) return "already have recourse";
					if (!BuffStackTest(buff, pSpellRecourse)) return "recourse does not stack";
				}
			}
			for (int b = 0; b < NUM_TEMP_BUFFS; b++)
			{
				PSPELL buff = GetSpellByID(GetPcProfile()->GetTempEffect(b).SpellID);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpell->ID) return "already have temp buff";
				if (!BuffStackTest(buff, pSpell)) return "does not stack";
				if (pSpellRecourse) {
					if (buff->ID == pSpellRecourse->ID) return "already have temp buff recourse";
					if (!BuffStackTest(buff, pSpellRecourse)) return "temp buff recourse does not stack";
				}
			}
		}

		return "";
	}

	if (pSpell->TargetType == 5 && pSpell->SpellType >= 1) { //single target beneficial spell
		if (GetCharInfo()->pGroupInfo != nullptr) {
			if (isHeal) { //single target heal
				int spawnPctHPs = 100;
				int spawnGroupID = -1;
				CGroupMember* pG;
				PSPAWNINFO pSpawn;
				for (int i = 0; i < 6; i++) {
					pG = GetCharInfo()->Group->GetGroupMember(i);
					if (!pG) continue;
					if (pG->Offline) continue;
					//if (${Group.Member[${i}].Mercenary}) /continue
					//if (${ Group.Member[${i}].OtherZone }) / continue
					pSpawn = pG->pSpawn;
					if (!pSpawn) continue;
					//if (pSpawn->SpawnID == pChar->pSpawn->SpawnID) continue;
					
					
					if (pSpawn->Type == SPAWN_CORPSE) continue;
					if (Distance3DToSpawn(pChar->pSpawn, pSpawn) > 200) continue;
					if (SpawnPctHPs(pSpawn) > spawnPctHPs) continue;
					spawnGroupID = i;
					spawnPctHPs = SpawnPctHPs(pSpawn);
				}

				if (spawnPctHPs >= HealAIMax) {
					sprintf_s(szTemp, "no one in group < %d%% hp (%d%% lowest %d)", HealAIMax, spawnPctHPs, spawnGroupID);
					return szTemp;
				}

				pG = GetCharInfo()->Group->GetGroupMember(spawnGroupID);
				if (!pG) {
					return "group member not found";
				}
				pSpawn = pG->pSpawn;
				if (!ActionSpawnTarget(pSpawn)) return "can't target group";

				return "";
			}
		}

		//no group
		if (ticks > 0) {
			for (int b = 0; b < MAX_TOTAL_BUFFS; b++)
			{
				PSPELL buff = GetSpellByID(GetPcProfile()->Buffs[b].SpellID);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpell->ID) return "already have buff";
				if (!BuffStackTest(buff, pSpell)) return "does not stack";
				if (pSpellRecourse) {
					if (buff->ID == pSpellRecourse->ID) return "already have recourse";
					if (!BuffStackTest(buff, pSpellRecourse)) return "recourse does not stack";
				}
			}
			for (int b = 0; b < NUM_TEMP_BUFFS; b++)
			{
				PSPELL buff = GetSpellByID(GetPcProfile()->GetTempEffect(b).SpellID);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpell->ID) return "already have temp effect";
				if (!BuffStackTest(buff, pSpell)) return "temp effect does not stack";
				if (pSpellRecourse) {
					if (buff->ID == pSpellRecourse->ID) return "already have temp effect recourse";
					if (!BuffStackTest(buff, pSpellRecourse)) return "temp effect recourse does not stack";
				}
			}
		}

		if (isHeal && SpawnPctHPs(pChar->pSpawn) >= HealAIMax) {
			sprintf_s(szTemp, "> %d%% hp", HealAIMax);
			return szTemp;
		}

		if (!ActionSpawnTarget(pChar->pSpawn)) return "can't target self";
		return "";
	}

	if (damageAmount > 0 && (pSpell->TargetType == 2 || pSpell->TargetType == 4)) { // AE DD
		ExtendedTargetList* xtm = pChar->pXTargetMgr;
		if (!xtm) return "xtarget not supported";
		if (!xtm->XTargetSlots.Count) return "xtarget no count found";

		
		int xTargetCount = 0;
		for (int n = 0; n < xtm->XTargetSlots.Count; n++)
		{
			XTARGETSLOT xts = xtm->XTargetSlots[n];
			if (xts.xTargetType != XTARGET_AUTO_HATER) continue;
			if (!xts.XTargetSlotStatus) continue;
			if (!xts.SpawnID) continue;

			PSPAWNINFO pXTargetSpawn = (PSPAWNINFO)GetSpawnByID(xts.SpawnID);
			if (!pXTargetSpawn) continue;
			if (pXTargetSpawn->Type != SPAWN_NPC) continue;
			if (Distance3DToSpawn(pChar->pSpawn, pXTargetSpawn) > pSpell->AERange) continue;
			if (SpawnPctHPs(pXTargetSpawn) < NukeAIMax) continue;

			xTargetCount++;
			//TODO: Main assist check
			//GetTargetBuffBySPA(31, 0)) // Is Target Mezzed?
		}
		if (xTargetCount < 3) {
			sprintf_s(szTemp, "< 3 targets nearby (%d)", xTargetCount);
			return szTemp;
		}
		return "";
	}
 
	if (isSingleTargetSpell && pSpell->SpellType == 0) { //single target detrimental spell
		if (!pTarget) return "no target";

		if (pTarget->Type != SPAWN_NPC) return "non npc targetted";

		if (pTarget && Distance3DToSpawn(pChar->pSpawn, (PSPAWNINFO)pTarget) > pSpell->AERange && Distance3DToSpawn(pChar->pSpawn, (PSPAWNINFO)pTarget) > pSpell->Range) return "target too far away";

		//if (pChar->pGroupInfo && !pChar->Group->GetGroupMember(0)->MainTank && SpawnPctHPs((PSPAWNINFO)pTarget) > NukeAIMax) {
		if (SpawnPctHPs(pTarget) > NukeAIMax) {
			sprintf_s(szTemp, "target > %d%%", NukeAIMax);
			return szTemp;
		}

		int mobHP = MobHP((PSPAWNINFO)pTarget);
		if (mobHP > 0 && damageAmount > 0 &&  damageAmount > mobHP) return "mob too low hp";

		if (isDebuff && !gTargetbuffs) return "target buffs not populated";

		if (IsHateAIRunning && damageAmount > 0 && SpawnPctHPs((PSPAWNINFO)pTarget) > 20 && IsHighHateAggro() && !IsIdealDamageReceiver()) return "waiting to damage until hate lowers";

		if (isTaunt) {
			if (GetCharInfo()->pGroupInfo == nullptr) return "not in a group";
			
			if (!pAggroInfo) return "pAggroInfo empty";

			bool isMainTank = false;
			CGroupMember* pG;
			PSPAWNINFO pSpawn;
			for (int i = 0; i < 6; i++) {
				pG = GetCharInfo()->Group->GetGroupMember(i);
				if (!pG) continue;
				if (pG->Offline) continue;
				//if (${Group.Member[${i}].Mercenary}) /continue
				//if (${ Group.Member[${i}].OtherZone }) / continue
				pSpawn = pG->pSpawn;
				if (!pSpawn) continue;
				if (pSpawn->SpawnID != pChar->pSpawn->SpawnID) continue;
				if (pSpawn->Type == SPAWN_CORPSE) continue;
				if (pG->MainTank > 0) {
					isMainTank = true;
					break;
				}
			}

			if (!isMainTank) return "not main tank";

			if (pAggroInfo->AggroTargetID == pChar->pSpawn->SpawnID) return "already primary target";
			if (pAggroInfo && pAggroInfo->aggroData[AD_Player].AggroPct >= 100) return "already have high hate";

		}

		if (stunDuration > 1000 && stunGlobalCooldown > std::chrono::steady_clock::now()) {
			sprintf_s(szTemp, "stun AI cooldown (%llds)", ((stunGlobalCooldown - std::chrono::steady_clock::now()) / 1000));
			return szTemp;
		}
		return "";
	}

	if (pSpell->TargetType == 0x0e || pSpell->Category == 69) { //Pet target
		PSPAWNINFO pPet = (PSPAWNINFO)GetSpawnByID(pChar->pSpawn->PetID);
		if (!pPet) return "no pet";

		//if (Distance3DToSpawn(pChar, pPet) > pSpell->Range) {
		//	Gems[gemIndex] = "pet too far away (" + to_string(Distance3DToSpawn(pChar, pPet)) + " / " + to_string(pSpell->Range) + ")";
		//	return false;
		//}
		
		if (isHeal) {
			if (SpawnPctHPs(pPet) > HealAIMax) {
				sprintf_s(szTemp, "pet > %d%% hp", HealAIMax);
				return szTemp;
			}
			return "";
		}

		if (pSpell->DurationCap > 0) { //pet buff
			if (!IsSpellStackable(pChar->pSpawn, pSpell)) return "not stackable";
			if (!pPetInfoWnd) return "no pet info wnd";
			if (pChar->pSpawn->PetID == 0) return "no pet";
			for (int nBuff = 0; nBuff < 30; nBuff++) {
				PSPELL buff = GetSpellByID(((PEQPETINFOWINDOW)pPetInfoWnd)->Buff[nBuff]);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpell->ID) return "pet already has buff";
				if (!BuffStackTest(buff, pSpell)) return "pet buff does not stack";
			}

			return "";
		}
	}

	return "unsupported spell";
}