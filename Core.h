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

bool IsCastingReady();
bool IsMoving(PSPAWNINFO pSpawn);
bool IsInSafeZone();
bool AreObstructionWindowsVisible();
bool HasBuff(PSPAWNINFO pSpawn, PCHAR szName);
bool IsCombatAbilityReady(PCHAR szName);
bool IsSpellReady(PCHAR szName);
bool IsSpellMemorized(PCHAR szName);
bool IsAbilityReady(PCHAR szName);
bool IsNavActive();
bool IsFacingSpawn(PSPAWNINFO pSpawn);

int CombatState();
int SpawnPctHPs(PSPAWNINFO pSpawn);
int XTargetNearbyHaterCount();
DWORD SpellCooldown(PCHAR szName);
PMQPLUGIN Plugin(char* plugin);



