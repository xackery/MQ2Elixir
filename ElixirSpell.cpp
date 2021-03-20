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
	bool isDoT = false;
	bool isLifetap = false;
	bool isSnare = false;
	bool isSow = false;
	bool isTaunt = false;

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
				isHeal = true;
				if (ticks > 0) {
					isBuff = true;
				}
				if (base < 0) {
					isHeal = false;
				}
				if (pSpell->Category == 126) { //Taps
					isLifetap = true;
				}
				//if (pSpell->Category == 114) { //Taps like touch of zlandicar
				//	isLifetap = true;
				//}
			}
			if (max < 0) { //Nuke / DoT
				if (ticks > 0) {
					isDoT = true;
				}
			}
		}
		if (attr == SPA_AC) {
			if (base > 0) { //+AC
				isBuff = true;
			}
			if (base < 0) { //-AC
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
		if (attr == 192) { //taunt
			isTaunt = true;
		}
	}

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
			if (!BuffStackTest(pBuffSpell, pSpell)) {
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
		 Gems[gemIndex] =  "not enough mana (" + to_string((int)pSpell->ManaCost) +  "/" + to_string(GetCharInfo2()->Mana) + ")";
		return false;
	}

	if (pSpell->TargetType == 6 && pSpell->SpellType >= 1) { //self beneficial spell
		if (pSpell->CastTime > 1000 && CombatState() == COMBATSTATE_COMBAT) {
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
						Gems[gemIndex] = "recourse already have buff";
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
		Execute("/cast %d", gemIndex + 1);
		return true;
	}

	if (pSpell->TargetType == 5 && pSpell->SpellType >= 1) { //single target beneficial spell
		if (isHeal) { //single target heal
			if (GetCharInfo()->pGroupInfo == nullptr) {
				Gems[gemIndex] = "not in a group";
				return false;
			}
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
			//pTarget = pSpawn;


			Gems[gemIndex] = "casting on party member";
			Execute("/cast %d", gemIndex + 1);
			return true;
		}
	}

	if (pSpell->TargetType == 5 && pSpell->SpellType == 0) { //single target detrimental spell
		if (!pTarget) {
			Gems[gemIndex] = "no target";
			return false;
		}

		if (pTarget->Data.Type != SPAWN_NPC) {
			Gems[gemIndex] = "non npc targetted";
			return false;
		}


		if (pTarget && Distance3DToSpawn(pChar->pSpawn, (PSPAWNINFO)pTarget) > pSpell->Range) {
			Gems[gemIndex] = "target too far away";
			return false;
		}

		if (SpawnPctHPs((PSPAWNINFO)pTarget) > 99) {
			Gems[gemIndex] = "target > 99%";
			return false;
		}

		if (isLifetap && SpawnPctHPs(pChar->pSpawn) > 50) {
			Gems[gemIndex] = "hp > 50, too high for lifetap";
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

	Gems[gemIndex] = "unsupported spell";
	return false;
}