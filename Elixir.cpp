#include "../MQ2Plugin.h"
#include "Core.h"
#include "Elixir.h"

using namespace std;

void Elixir::OnPulse()
{

	int maxGems = 8;
	if (PALTABILITY pAbility = AAByName("Mnemonic Retention")) {
		int rank = pAbility->CurrentRank - 1;
#if !defined(ROF2EMU) && !defined(UFEMU)
		if (((PcZoneClient*)pPCData)->HasAlternateAbility(pAbility->Index, NULL, false, false))
#else
		if (((PcZoneClient*)pPCData)->HasAlternateAbility(pAbility->Index, NULL, false))
#endif
		{
			rank++;
		}
		if (rank > 0 && rank < 10) {
			maxGems += rank;
		}
	}

	for (int i = 0; i < maxGems-1; i++) {
		AttemptGem(i);
	}
	for (int i = maxGems; i < NUM_SPELL_GEMS; i++) {
		Gems[i] = "not available";
	}
	
	for (int i = 0; i < 10; i++) {
		AttemptButton(i);
	}

	//TODO: buff cycle last gem (maxGem)
}

void Elixir::AttemptGem(int gemIndex)
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) {
		Gems[gemIndex] = "char not loaded";
		return;
	}

	if (!pLocalPlayer) {
		Gems[gemIndex] = "pLocalPlayer not loaded";
		return;
	}

	if (isActionComplete) {
		Gems[gemIndex] = "earlier action completed";
		return;
	}

	if (pChar->pSpawn->GetClass() != Bard && IsMoving(pChar->pSpawn)) {
		Gems[gemIndex] = "player is moving";
		return;
	}

	if (gemGlobalCooldown > MQGetTickCount64()) {
		Gems[gemIndex] = "gem AI cooldown";
		return;
	}

	if (AreObstructionWindowsVisible()) {
		Gems[gemIndex] = "obstruction window visible";
		return;
	}

	if (GetCharInfo()->pSpawn->CastingData.IsCasting()) {
		Gems[gemIndex] = "already casting";
		return;
	}

	
	if ((int)((PCDISPLAY)pDisplay)->TimeStamp <= ((PSPAWNINFO)pLocalPlayer)->GetSpellCooldownETA()) {
		Gems[gemIndex] = "all gems on recent use cooldown";
		return;
	}

	if (((PCDISPLAY)pDisplay)->TimeStamp <= ((PSPAWNINFO)pLocalPlayer)->SpellGemETA[gemIndex]) {
		Gems[gemIndex] = "gem on cooldown";
		return;
	}

	if (lastGemIndex == gemIndex && lastActionRepeatCooldown > MQGetTickCount64()) {
		Gems[gemIndex] = "last action repeat cooldown";
		return;
	}

	if (!Spell(gemIndex)) {
		Gems[gemIndex] = "spell: " + Gems[gemIndex];
		return;
	}

	lastAction = Gems[gemIndex];
	lastActionRepeatCooldown = (unsigned long)MQGetTickCount64() + 3000;
	lastGemIndex = gemIndex;
	isActionComplete = true;
	gemGlobalCooldown = (unsigned long)MQGetTickCount64() + 3000;
}

void Elixir::AttemptButton(int buttonIndex)
{
	PCHARINFO pChar = GetCharInfo();
	if (!pChar) {
		buttons[buttonIndex] = "char not loaded";
		return;
	}

	if (pChar->pSpawn->GetClass() != Bard && isActionComplete) {
		buttons[buttonIndex] = "earlier action complete";
		return;
	}

	if (lastButtonIndex == buttonIndex && lastActionRepeatCooldown > MQGetTickCount64()) {
		buttons[buttonIndex] = "last action repeat cooldown";
		return;
	}

	lastAction = "pressing hotbutton " + buttonIndex;
	buttons[buttonIndex] = lastAction;
	lastActionRepeatCooldown = (unsigned long)MQGetTickCount64() + 3000;
	lastButtonIndex = buttonIndex;
	isActionComplete = true;
}

bool Elixir::Spell(int gemIndex)
{
	PCHARINFO pChar = GetCharInfo();
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
					isLifetap = true;
				}
			}
			if (max < 0) { //Nuke / DoT
				if (ticks > 0) {
					isDoT = true;
				}

				return true;
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
	}


	if (!pSpell->CanCastInCombat && CombatState() == COMBATSTATE_COMBAT) {
		Gems[gemIndex] = "cannot be cast in combat";
		return false;
	}

	if (pChar->pSpawn->GetCurrentMana() < (int)pSpell->ManaCost) {
		Gems[gemIndex] = "not enough mana";
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
			}
		}
		Gems[gemIndex] = "casting on self";
		Execute("/cast %d", gemIndex + 1);
		return true;
	}

	if (pSpell->TargetType == 5 && pSpell->SpellType >= 1) { //single target beneficial spell
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

		//TODO: target pctHPs < 99%

		if (isLifetap && SpawnPctHPs(pChar->pSpawn) > 50) {
			Gems[gemIndex] = "hp > 50, too high for lifetap";
			return false;
		}

		if (isDebuff || ticks > 0) {
			/*for (int b = 0; b < NUM_LONG_BUFFS; b++)
			{
				PSPELL buff = GetSpellByID(pTarget.spawn(PSPAWNINFO)pTarget)  GetCharInfo2()->Buff[b].SpellID);
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
			}*/
		}

		Gems[gemIndex] = "casting on enemy target";
		Execute("/cast %d", gemIndex + 1);
		return true;
	}

	Gems[gemIndex] = "unsupported spell";
	return false;
}