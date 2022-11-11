#pragma once

#include "../MQ2Plugin.h"
#include "Core.h"
// Elixir is the brains of the plugin
class Elixir {
public:

	// StanceMode is what mode is currently being used.
	int StanceMode = 1;
	// cooldown before doing anything after zoning
	std::chrono::steady_clock::time_point ZoneCooldownTimer;

	string LastAction;
	string Gems[NUM_SPELL_GEMS];
	string Buttons[12];
	string TargetStr;
	void OnPulse();

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
	// Tag Mode is a debugging display on spell tooltips etc that show the tags associated with a spell
	bool IsDebugTagMode = false;
	
	// Is Nuke AI logic running. When disabled, enemies don't get hit by offensive abilities
	bool IsNukeAIRunning = false;
	// NukeAI's Maximum HP Value before casting spells
	int NukeAIMax = 95;

	// Is Target AI logic running. When disabled, targetting main assist no longer happens
	bool IsTargetAIRunning = false;
	// TargetAI's minimum range a target must be for assist to trigger
	int TargetAIMinRange = 50;

	// If Settings Debug is Enabled, shows additional debug strings on the window
	bool IsSettingsDebugEnabled = false;

	// Is the target considered having high hate
	bool IsHighHateAggro();

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

	// Gem attempts to cast the provided gem index
	void ActionGem(int gemIndex);
	void ActionButton(int buttonIndex);
	void ActionTarget();

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

