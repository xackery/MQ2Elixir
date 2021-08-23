#pragma once

#include "../MQ2Plugin.h"

using namespace std;

// MobHP returns the most accurate HP elixir knows for a mob, first trying dannet for players, and HPEstimate as a fallback after
int MobHP(PSPAWNINFO pSpawn);
// MobHPDannet returns the HP of a player, or 0 if non-player/a peer not running dannet
int MobHPDannet(PSPAWNINFO pSpawn);
// MobHPEstimate uses eqemu coding to estimate the max hp of a mob
int MobHPEstimate(PSPAWNINFO pSpawn);

// IsPlayerPoisoned returns true if a poison is detected on a mob. It uses any available methods to determine this, namely dannet
bool IsPlayerPoisoned(PSPAWNINFO pSpawn);
// IsPlayerPoisonedDannet returns true when a poison is detected via /dobs, returns false if dannet is not enabled etc
bool IsPlayerPoisonedDannet(PSPAWNINFO pSpawn);