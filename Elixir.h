#pragma once

#include "../MQ2Plugin.h"
#include "Core.h"
// Elixir is the brains of the plugin
class Elixir {
public:

	// StanceMode is what mode is currently being used.
	int StanceMode = 1;
	// cooldown before doing anything after zoning
	unsigned long ZoneCooldown;

	string LastAction;
	string Gems[NUM_SPELL_GEMS];
	string Buttons[12];
	void OnPulse();

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
	
	unsigned long gemGlobalCooldown;
	
	int lastGemIndex;
	int lastButtonIndex;
	int lastActionRepeat;
	unsigned long lastActionRepeatCooldown;
	

	// Gem attempts to cast the provided gem index
	void ActionGem(int gemIndex);
	void ActionButton(int buttonIndex);

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

