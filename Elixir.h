#pragma once

#include "../MQ2Plugin.h"
#include "Core.h"
// Elixir is the brains of the plugin
class Elixir {
public:

	const char* ElixirPluginVersion = "Elixir v0.1.4";
	// cooldown before doing anything after zoning
	std::chrono::steady_clock::time_point ZoneCooldownTimer;

	string LastAction;
	string Gems[NUM_SPELL_GEMS];
	bool IgnoreGems[NUM_SPELL_GEMS];
	int ignoreGemCount;
	int maxGemCount;
	string Buttons[12];
	void OnPulse();

	// Is Elixir as a whole running or not, master override
	bool IsElixirRunning = true;

	// Is Elixir UI Showing?
	bool IsElixirUIShowing = true;

	// Will elixir AI run if the EQ window is focused?
	bool IsElixirDisabledOnFocus = false;

	// Is Hate AI logic running. When disabled, hate is not honored and we just nuke freely
	bool IsHateAIRunning = false;
	// HateAI's Maximum Hate threshold before it stops doing offensive abilities and spells
	int HateAIMax = 80;

	// Is Heal AI logic running. When disabled. heals will not be casted
	bool IsHealAIRunning = false;
	// HealAI Maximum health value to trigger heal spells to be casted on a group member
	int HealAIMax = 50;
	
	// Is Buff AI logic running. When disabled, buffs will not be casted
	bool IsBuffAIRunning = false;
	// Is Buff attempting during combat
	bool IsBuffDuringCombat = false;

	// Tag Mode is a debugging display on spell tooltips etc that show the tags associated with a spell
	bool IsDebugTagMode = false;
	
	// Is Nuke AI logic running. When disabled, enemies don't get hit by offensive abilities
	bool IsNukeAIRunning = false;
	// NukeAI's Maximum HP Value before casting spells
	int NukeAIHPMax = 95;
	// NukeAI's Maximum Mana Value before stopping casting spells
	int NukeAIManaMax = 50;

	// Is Meditate AI logic running. When disabled, sitting behaviors won't trigger
	bool IsMeditateAIRunning = false;
	string MeditateAIStr;
	bool IsMeditateBySitting = false;
	bool IsMeditateEnabledDuringCombat = true;
	int lastHPOnSit = 0;
	std::chrono::steady_clock::time_point sitCooldownTimer;
	bool IsMeditateByCannibalize = false;
	int CanniMaxMana = 99;
	int CanniMinHP = 20;

	// Is Target AI logic running. When disabled, targetting main assist no longer happens
	bool IsTargetAIRunning = false;
	string TargetAIStr;
	// TargetAI's minimum range a target must be for assist to trigger
	int TargetAIMinRange = 50;
	// Is Target Auto Attacking allowed, if done via assist
	bool IsTargetAutoAttack = false;
	// Is Target Pet Attacking allowed, if done via assist
	bool IsTargetPetAttack = false;

	// If Settings Debug is Enabled, shows additional debug strings on the window
	bool IsSettingsDebugEnabled = false;

	bool IsCharmAIRunning = false;
	string CharmAIStr;
	int CharmSpawnID = 0;
	char CharmName[64] = "No Target";
	bool IsCharmCurrentValid = false;
	std::chrono::steady_clock::time_point charmCooldownTimer;

	// Is the target considered having high hate
	bool IsHighHateAggro();
	void ClearCharmTarget();
	/*
	virtual bool CastGroupHeal(PSPAWNINFO pSpawn) { return false; }
	virtual bool CastPriorityHeal(PSPAWNINFO pSpawn) { return false; }
	virtual bool CastGroupPriorityHeal(PSPAWNINFO pSpawn) { return false; }
	virtual bool CastHeal(PSPAWNINFO pSpawn) { return false; }
	virtual bool CastCombatBuff() { return false; }
	virtual bool CastIdleBuff() { return false; }
	
	bool Logic();
	*/
private:

	int highPctHPs = 90;
	int lowPctHPs = 45;
	// isActionComplete is set to false on every pulse. When an action succeeds, this is flagged, stopping future actions.
	bool isActionComplete;
	
	std::chrono::steady_clock::time_point gemGlobalCooldown;
	std::chrono::steady_clock::time_point movementGlobalCooldown;
	// if I cast a stun, this is a cooldown before using another follow up one
	std::chrono::steady_clock::time_point stunGlobalCooldown;

	// lastCastedSpellID only is set when elixir invokes a spell cast
	int lastCastedSpellID;
	
	int lastGemIndex;
	int lastButtonIndex;
	int lastActionRepeat;
	std::chrono::steady_clock::time_point lastActionRepeatCooldown;
	
	std::chrono::steady_clock::time_point lastTargetRepeatCooldownTimer;

	// Last time charm was used, this gem index was it
	int lastCharmGemIndex;
	// Last time charm was used, this was the spell ID on the gem index
	int lastCharmSpellID;
	// Gem attempts to cast the provided gem index
	void ActionGem(int gemIndex);
	void ActionButton(int buttonIndex);
	void ActionTarget();
	void ActionSit();
	void ActionCharm();

	bool IsCharm(PSPELL pSpell);
	std::string Elixir::Ability(int abilityIndex);
	std::string Elixir::Spell(PSPELL pSpell);	
	std::string Elixir::CombatAbility(PSPELL pSpell);

	/*
	bool Target();
	bool Spell();
	bool Ability();
	bool Item();
	bool Heal();
	bool SelfHeal();
	bool GroupHeal();
	bool RaidHeal();	
	bool Buff();
	bool IdleBuff();
	bool CombatBuff();*/
};

