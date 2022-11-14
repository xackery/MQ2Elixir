#pragma once
using namespace std;

enum {
	COMBATSTATE_NULL = -1,
	COMBATSTATE_COMBAT,
	COMBATSTATE_DEBUFFED,
	COMBATSTATE_COOLDOWN,
	COMBATSTATE_ACTIVE,
	COMBATSTATE_RESTING,
	COMBATSTATE_UNKNOWN
};

bool ActionCastGem(int gemIndex);
bool ActionCast(PCHAR szName);
bool ActionCastSpell(PCHAR szName);
bool ActionSpawnTarget(PSPAWNINFO pSpawn);
bool ActionCastAA(PCHAR szName);
bool ActionCastItem(PCHAR szName);
bool ActionMemorizeSpell(WORD gem, PCHAR szName);
bool ActionCastAbility(PCHAR szName);
bool ActionCastCombatAbility(PCHAR szName);

void BuffUpdate(PSPAWNINFO pSpawn);
void Execute(PCHAR zFormat, ...);

PCONTENTS EquippedSlot(PCONTENTS pCont);
bool IsCastingReady();
bool IsMoving(PSPAWNINFO pSpawn);
bool IsInSafeZone();
bool AreObstructionWindowsVisible();
bool HasBuff(PSPAWNINFO pSpawn, PCHAR szName);
bool IsCombatAbilityReady(PCHAR szName);
bool IsSpellReady(PCHAR szName);
bool IsIdealDamageReceiver();
bool IsSpellMemorized(PCHAR szName);
bool IsAbilityReady(PCHAR szName);
bool IsNavActive();
bool IsFacingSpawn(PSPAWNINFO pSpawn);
bool IsAAPurchased(PCHAR AAName);
bool IsSpellStackable(PSPAWNINFO pSpawn, PSPELL pSpell);
bool IsSpellStackableCompare(PSPELL pSpell1, PSPELL pSpell2);
PALTABILITY AAByName(PCHAR Name);
int CombatState();
int PctMana();
int PctHPs();
int SpawnPctHPs(PSPAWNINFO pSpawn);
int SpawnPctMana(PSPAWNINFO pSpawn);
unsigned int PetTargetID();
unsigned long StunDuration(PSPELL pSpell);
unsigned long MainAssistTargetID();
#if !defined(ROF2EMU) && !defined(UF2EMU)
int XTargetNearbyHaterCount();
#endif
DWORD SpellCooldown(PCHAR szName);
MQPlugin* Plugin(char* plugin);



