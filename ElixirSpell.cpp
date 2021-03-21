#include "../MQ2Plugin.h"
#include "Core.h"
#include "Elixir.h"

using namespace std;

bool Elixir::Spell(int gemIndex)
{
	CHAR szTemp[MAX_STRING] = { 0 };

	PCHARINFO pChar = (PCHARINFO)pCharData;
	if (!pChar) {
		Gems[gemIndex] = "char not loaded";
		return false;
	}

	if (!pLocalPlayer) {
		Gems[gemIndex] = "pLocalPlayer not loaded";
		return false;
	}

	LONG spellID = GetMemorizedSpell(gemIndex);
	if (!spellID) {
		Gems[gemIndex] = "no spell memorized";
		return false;
	}

	PSPELL pSpell = GetSpellByID(spellID);
	if (!pSpell) {
		Gems[gemIndex] = "spell not found";
		return false;
	}

	bool isHeal = false;
	bool isDebuff = false;
	bool isBuff = false;
	bool isDamage = false;
	bool isLifetap = false;
	bool isSnare = false;
	bool isSow = false;
	bool isTaunt = false;
	bool isSingleTargetSpell = false;
	bool isPetSummon = false;
	bool isTransport = false;
	bool isGroupSpell = false;
	bool isBardSong = false;
	
	int bodyType;
	LONG attr;
	LONG base;
	LONG base2;
	LONG max;
	LONG calc;
	LONG spellgroup = pSpell->SpellGroup;
	LONG ticks = pSpell->DurationCap;
	LONG targets = pSpell->MaxTargets;
	LONG targettype = pSpell->TargetType;
	LONG skill = pSpell->Skill;
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

		if (attr == SPA_HP) {
			if (max > 0) { //Heal / HoT
				if (ticks < 5 && base > 0) { //regen etc are not heals
					isHeal = true;
				}
				if (ticks > 0) {
					isBuff = true;
				}
				if (pSpell->Category == 114) { //Taps like touch of zlandicar
					isLifetap = true;
				}
			}
			if (max < 0) { //Nuke / DoT
				isDamage = true;
			}
		}
		if (attr == SPA_AC ||
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

		if (attr == SPA_MOVEMENTRATE) {
			if (base > 0) { //+Movement
				isBuff = true;
				isSow = true;
			}
			if (base < 0) { //-Movement
				isDebuff = true;
				isSnare = true;
			}
		}
		
		if (attr == SPA_CHA) {
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

		if (attr == 26) { // Gate
			isTransport = true;
		}
		if (attr == 30) { // frenzy radius reduction (lull)
			Gems[gemIndex] = "ignored (Lull)";
			return false;
		}
		if (attr == 86) { // reaction radius reduction (lull)
			Gems[gemIndex] = "ignored (Lull)";
			return false;
		}
		if (attr == 18) { // pacify (lull)
			Gems[gemIndex] = "ignored (Lull)";
			return false;
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

		if (attr == 108) { //Summon Familiar
			isTaunt = true;
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
	if (pSpell->TargetType == 4) { //PB AE
		isSingleTargetSpell = true; //for now just treat like a single target spell
	}

	if (pSpell->TargetType == 5) { //Single
		isSingleTargetSpell = true;
	}

	if (isTransport) {
		Gems[gemIndex] = "ignoring (transport)";
		return false;
	}

	if (pTarget) {
		bodyType = GetBodyType((PSPAWNINFO)pTarget);
	}
	if (pSpell->TargetType == 16) { //Animal
		if (!pTarget) {
			Gems[gemIndex] = "no target";
			return false;
		}
		if (bodyType != 21) {
			Gems[gemIndex] = "not animal";
		}
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 10) { //Undead
		if (!pTarget) {
			Gems[gemIndex] = "no target";
			return false;
		}
		if (bodyType != 3) {
			Gems[gemIndex] = "not undead";
		}
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 11) { //Summoned
		if (!pTarget) {
			Gems[gemIndex] = "no target";
			return false;
		}
		if (bodyType != 28) {
			Gems[gemIndex] = "not summoned";
		}
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
		if (!pTarget) {
			Gems[gemIndex] = "no target";
			return false;
		}
		if (bodyType != 25) {
			Gems[gemIndex] = "not plant";
		}
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 17) { //Uber Giants
		isSingleTargetSpell = true;
	}

	if (pSpell->TargetType == 18) { //Uber Dragons
		isSingleTargetSpell = true;
	}

	if (pSpell->NoNPCLOS == 0 && pTarget && isSingleTargetSpell && !pCharSpawn->CanSee((EQPlayer*)pTarget)) {
		Gems[gemIndex] = "no line of sight";
		return false;
	}


	for (int i = 0; i < 4; i++) { // Reagent check
		if (pSpell->ReagentCount[i] == 0) continue;
		if (pSpell->ReagentID[i] == -1) continue;

		DWORD count = FindItemCountByID((int)pSpell->ReagentID[i]);
		if (count > pSpell->ReagentCount[i]) continue;

		PCHAR pItemName = GetItemFromContents(FindItemByID((int)pSpell->ReagentID[i]))->Name;
		if (pItemName) {
			sprintf(szTemp, "missing %dx %s", pSpell->ReagentCount[i], pItemName);
			Gems[gemIndex] = szTemp;
			return false;
		}
	}


	DWORD ReqID = pSpell->CasterRequirementID;
	if (ReqID == 518 && SpawnPctHPs(pChar->pSpawn) > 89) {
		Gems[gemIndex] = "not < 90% hp";
		return false;
	}
	//if (ReqID == 825 && SpawnPctEndurance(pChar->pSpawn) > 20) return false;
	//if (ReqID == 826 && SpawnPctEndurance(pChar->pSpawn) > 24) return false;
	//if (ReqID == 827 && SpawnPctEndurance(pChar->pSpawn) > 29) return false;

	if (pSpellRecourse && pSpellRecourse->DurationCap > 0) { //recourse buff attached
		for (unsigned long nBuff = 0; nBuff < NUM_LONG_BUFFS; nBuff++) {
			if (pSpellRecourse->TargetType == 6) { //self buff
				PSPELL buff = GetSpellByID(GetCharInfo2()->Buff[nBuff].SpellID);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpellRecourse->ID) {
					Gems[gemIndex] = "recourse already have buff";
					return false;
				}
				if (!BuffStackTest(buff, pSpellRecourse)) {
					Gems[gemIndex] = "recourse does not stack";
					return false;
				}
			}

			int buffid = ((PCTARGETWND)pTargetWnd)->BuffSpellID[nBuff];
			if (buffid <= 0) {
				continue;
			}
			PSPELL pBuffSpell = GetSpellByID(buffid);
			if (!pBuffSpell) {
				continue;
			}

			DWORD buffDuration = ((PCTARGETWND)pTargetWnd)->BuffTimer[nBuff];
			if (buffDuration <= 0) {
				continue;
			}
			if (!BuffStackTest(pBuffSpell, pSpellRecourse)) {
				Gems[gemIndex] = "debuff already on";
				return false;
			}
			if (pSpellRecourse->TargetType == 8) { //targetted
				if (pBuffSpell->ID == pSpellRecourse->ID) {
					Gems[gemIndex] = "recourse already on target";
					return false;
				}
				if (!BuffStackTest(pBuffSpell, pSpellRecourse)) {
					Gems[gemIndex] = "recourse does not stack on target";
					return false;
				}
			}
		}
	}


	if (ticks > 0 && pSpell->SpellType == 0) { // debuff
		for (unsigned long nBuff = 0; nBuff < NUM_LONG_BUFFS; nBuff++) {
			
			int buffid = ((PCTARGETWND)pTargetWnd)->BuffSpellID[nBuff];
			if (buffid <= 0) {
				continue;
			}
			PSPELL pBuffSpell = GetSpellByID(buffid);
			if (!pBuffSpell) {
				continue;
			}

			DWORD buffDuration = ((PCTARGETWND)pTargetWnd)->BuffTimer[nBuff];
			if (buffDuration <= 0) {
				continue;
			}
			if (!BuffStackTest(pBuffSpell, pSpell)) {
				Gems[gemIndex] = "debuff already on";
				return false;
			}

			if (pBuffSpell->ID == pSpell->ID) {
				Gems[gemIndex] = "debuff already on target";
				return false;
			}
			if (!BuffStackTest(pBuffSpell, pSpell)) {
				Gems[gemIndex] = "debuff does not stack on target";
				return false;
			}
		}
	}

	PZONEINFO pZone = reinterpret_cast<PZONEINFO>(pZoneInfo);
	if (pZone && pSpell->ZoneType == 1) { //can't cast indoors
		if (pZone->OutDoor == EOutDoor::IndoorDungeon) {
			Gems[gemIndex] = "in dungeon";
			return false;
		}
		if (pZone->OutDoor == EOutDoor::IndoorCity) {
			Gems[gemIndex] = "in city";
			return false;
		}
		if (pZone->OutDoor == EOutDoor::DungeonCity) {
			Gems[gemIndex] = "in dungeon city";
			return false;
		}
	}

	if (pZone && pSpell->ZoneType == 2) { //can't cast outdoor
		if (pZone->OutDoor == EOutDoor::Outdoor) {
			Gems[gemIndex] = "outdoors";
			return false;
		}
		if (pZone->OutDoor == EOutDoor::OutdoorCity) {
			Gems[gemIndex] = "outdoor city";
			return false;
		}
		if (pZone->OutDoor == EOutDoor::OutdoorDungeon) {
			Gems[gemIndex] = "outdoor dungeon";
			return false;
		}
	}


	if (!pSpell->CanCastInCombat && CombatState() == COMBATSTATE_COMBAT) {
		Gems[gemIndex] = "cannot be cast in combat";
		return false;
	}
	
	
	if (GetCharInfo2()->Mana < (int)pSpell->ManaCost) {
		Gems[gemIndex] = "not enough mana (" + to_string((int)pSpell->ManaCost) +  "/" + to_string(GetCharInfo2()->Mana) + ")";
		return false;
	}

	if (GetCharInfo2()->Endurance < (int)pSpell->EnduranceCost) {
		Gems[gemIndex] = "not enough endurance (" + to_string((int)pSpell->EnduranceCost) + "/" + to_string(GetCharInfo2()->Endurance) + ")";
		return false;
	}

	if (strstr(pSpell->Name, "Discipline") && pCombatAbilityWnd) {
		CXWnd* Child = ((CXWnd*)pCombatAbilityWnd)->GetChildItem("CAW_CombatEffectLabel");
		if (!Child) {
			Gems[gemIndex] = "combatabilitywnd not found";
			return false;
		}
		CHAR szBuffer[2048] = { 0 };
		GetCXStr(Child->CGetWindowText(), szBuffer, MAX_STRING);
		if (szBuffer[0] == '\0') {
			Gems[gemIndex] = "combatabilitywnd text not null terminated";
			return false;
		}
		PSPELL pBuff = GetSpellByName(szBuffer);
		if (pBuff) {
			sprintf(szTemp, "disc %s already active", pBuff->Name);
			Gems[gemIndex] = szTemp;
			return false;
		}
	}

	if (isPetSummon && pChar->pSpawn->PetID > 0) {
		Gems[gemIndex] = "already have pet";
		return false;
	}


	if (isLifetap && SpawnPctHPs(pChar->pSpawn) > 80) {
		Gems[gemIndex] = "> 80% hp";
		return false;
	}

	if (pSpell->SpellType >= 1 && (pSpell->TargetType == 6 || isGroupSpell)) { //self/group beneficial spell
		if (pSpell->CastTime > 1000 && CombatState() == COMBATSTATE_COMBAT && !isBardSong) {
			Gems[gemIndex] = "cast time > 1s, long for combat";
			return false;
		}

		if (ticks > 0) {
			for (int b = 0; b < NUM_LONG_BUFFS; b++)
			{
				PSPELL buff = GetSpellByID(GetCharInfo2()->Buff[b].SpellID);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpell->ID) {
					Gems[gemIndex] = "already have buff";
					return false;
				}
				if (!BuffStackTest(buff, pSpell)) {
					Gems[gemIndex] = "does not stack";
					return false;
				}
				if (pSpellRecourse) {
					if (buff->ID == pSpellRecourse->ID) {
						Gems[gemIndex] = "already have recourse";
						return false;
					}
					if (!BuffStackTest(buff, pSpellRecourse)) {
						Gems[gemIndex] = "recourse does not stack";
						return false;
					}
				}
			}
			for (int b = 0; b < NUM_SHORT_BUFFS; b++)
			{
				PSPELL buff = GetSpellByID(GetCharInfo2()->ShortBuff[b].SpellID);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpell->ID) {
					Gems[gemIndex] = "already have shortbuff";
					return false;
				}
				if (!BuffStackTest(buff, pSpell)) {
					Gems[gemIndex] = "does not stack";
					return false;
				}
				if (pSpellRecourse) {
					if (buff->ID == pSpellRecourse->ID) {
						Gems[gemIndex] = "already have short recourse";
						return false;
					}
					if (!BuffStackTest(buff, pSpellRecourse)) {
						Gems[gemIndex] = "recourse does not stack";
						return false;
					}
				}
			}
		}
		Gems[gemIndex] = "casting on self";

		if (isGroupSpell) {
			Gems[gemIndex] = "casting on group";
		}
		Execute("/cast %d", gemIndex + 1);
		return true;
	}

	if (pSpell->TargetType == 5 && pSpell->SpellType >= 1) { //single target beneficial spell
		if (GetCharInfo()->pGroupInfo != nullptr) {
			if (isHeal) { //single target heal
				int spawnPctHPs = 100;
				int spawnGroupID = -1;
				GROUPMEMBER* pG;
				PSPAWNINFO pSpawn;
				for (int i = 1; i < 6; i++) {
					pG = GetCharInfo()->pGroupInfo->pMember[i];
					if (!pG) continue;
					if (pG->Offline) continue;
					//if (${Group.Member[${i}].Mercenary}) /continue
					//if (${ Group.Member[${i}].OtherZone }) / continue
					pSpawn = pG->pSpawn;
					if (!pSpawn) continue;
					//if (pSpawn->SpawnID == pChar->pSpawn->SpawnID) continue;
					if (pSpawn->Type == SPAWN_CORPSE) continue;
					if (Distance3DToSpawn(pChar, pSpawn) > 200) continue;
					if (SpawnPctHPs(pSpawn) > spawnPctHPs) continue;
					spawnGroupID = i;
				}
				if (spawnPctHPs >= 50) {
					Gems[gemIndex] = "no one in range less than 50% hp";
					return false;
				}

				pG = GetCharInfo()->pGroupInfo->pMember[spawnGroupID];
				pSpawn = pG->pSpawn;
				if (!ActionSpawnTarget(pSpawn)) {
					Gems[gemIndex] = "can't target group";
					return false;
				}

				Gems[gemIndex] = "casting heal on party member";
				Execute("/cast %d", gemIndex + 1);
				return true;
			}
		}

		//no group
		if (ticks > 0) {
			for (int b = 0; b < NUM_LONG_BUFFS; b++)
			{
				PSPELL buff = GetSpellByID(GetCharInfo2()->Buff[b].SpellID);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpell->ID) {
					Gems[gemIndex] = "already have buff";
					return false;
				}
				if (!BuffStackTest(buff, pSpell)) {
					Gems[gemIndex] = "does not stack";
					return false;
				}
				if (pSpellRecourse) {
					if (buff->ID == pSpellRecourse->ID) {
						Gems[gemIndex] = "already have recourse";
						return false;
					}
					if (!BuffStackTest(buff, pSpellRecourse)) {
						Gems[gemIndex] = "recourse does not stack";
						return false;
					}
				}
			}
			for (int b = 0; b < NUM_SHORT_BUFFS; b++)
			{
				PSPELL buff = GetSpellByID(GetCharInfo2()->ShortBuff[b].SpellID);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpell->ID) {
					Gems[gemIndex] = "already have shortbuff";
					return false;
				}
				if (!BuffStackTest(buff, pSpell)) {
					Gems[gemIndex] = "does not stack";
					return false;
				}
				if (pSpellRecourse) {
					if (buff->ID == pSpellRecourse->ID) {
						Gems[gemIndex] = "already have short recourse";
						return false;
					}
					if (!BuffStackTest(buff, pSpellRecourse)) {
						Gems[gemIndex] = "recourse does not stack";
						return false;
					}
				}
			}
		}

		if (isHeal && SpawnPctHPs(pChar->pSpawn) >= 50) {
			Gems[gemIndex] = "I'm > 50% hp";
			return false;
		}

		if (!ActionSpawnTarget(pChar->pSpawn)) {
			Gems[gemIndex] = "can't target self";
			return false;
		}

		Gems[gemIndex] = "casting buff on self";
		Execute("/cast %d", gemIndex + 1);
		return true;
	}

	if (isSingleTargetSpell && pSpell->SpellType == 0) { //single target detrimental spell
		if (!pTarget) {
			Gems[gemIndex] = "no target";
			return false;
		}

		if (pTarget->Data.Type != SPAWN_NPC) {
			Gems[gemIndex] = "non npc targetted";
			return false;
		}

		if (pTarget && Distance3DToSpawn(pChar->pSpawn, (PSPAWNINFO)pTarget) > pSpell->AERange && Distance3DToSpawn(pChar->pSpawn, (PSPAWNINFO)pTarget) > pSpell->Range) {
			Gems[gemIndex] = "target too far away";
			return false;
		}

		if (SpawnPctHPs((PSPAWNINFO)pTarget) > 99) {
			Gems[gemIndex] = "target > 99%";
			return false;
		}

		if (isTaunt) {
			if (GetCharInfo()->pGroupInfo == nullptr) {
				Gems[gemIndex] = "not in a group";
				return false;
			}
			
			if (!pAggroInfo) {
				Gems[gemIndex] = "pAggroInfo empty";
				return false;
			}

			bool isMainTank = false;
			GROUPMEMBER* pG;
			PSPAWNINFO pSpawn;
			for (int i = 1; i < 6; i++) {
				pG = GetCharInfo()->pGroupInfo->pMember[i];
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

			if (!isMainTank) {
				Gems[gemIndex] = "I am not main tank";
				return false;
			}

			if (pAggroInfo->AggroTargetID == pChar->pSpawn->SpawnID) {
				Gems[gemIndex] = "I am already primary target";
				return false;
			}

		}

		Gems[gemIndex] = "casting on enemy target";
		Execute("/cast %d", gemIndex + 1);
		return true;
	}

	if (pSpell->TargetType == 0x0e || pSpell->Category == 69) { //Pet target
		PSPAWNINFO pPet = (PSPAWNINFO)GetSpawnByID(pChar->pSpawn->PetID);
		if (!pPet) {
			Gems[gemIndex] = "no pet";
			return false;
		}
		
		//if (Distance3DToSpawn(pChar, pPet) > pSpell->Range) {
		//	Gems[gemIndex] = "pet too far away (" + to_string(Distance3DToSpawn(pChar, pPet)) + " / " + to_string(pSpell->Range) + ")";
		//	return false;
		//}

		if (isHeal) {
			if (SpawnPctHPs(pPet) > 80) {
				Gems[gemIndex] = "pet > 80% hp";
				return false;
			}
			Gems[gemIndex] = "casting heal on pet";
			Execute("/cast %d", gemIndex + 1);
			return true;
		}

		if (pSpell->DurationCap > 0) { //pet buff
			if (!pPetInfoWnd) {
				Gems[gemIndex] = "no pet info wnd";
				return false;
			}
			if (pChar->pSpawn->PetID == 0) {
				Gems[gemIndex] = "no pet";
			}
			for (int nBuff = 0; nBuff < 30; nBuff++) {
				PSPELL buff = GetSpellByID(((PEQPETINFOWINDOW)pPetInfoWnd)->Buff[nBuff]);
				if (!buff) {
					continue;
				}
				if (buff->ID == pSpell->ID) {
					Gems[gemIndex] = "already have buff";
					return false;
				}
				if (!BuffStackTest(buff, pSpell)) {
					Gems[gemIndex] = "does not stack";
					return false;
				}
			}

			Gems[gemIndex] = "casting on pet";
			Execute("/cast %d", gemIndex + 1);
			return true;
		}
	}

	Gems[gemIndex] = "unsupported spell";
	return false;
}