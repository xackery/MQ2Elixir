#pragma once

#include "Elixir.h"

unsigned long PulseDelay;
bool IsZoning;
// exposed by members IsElixirActive
bool IsElixirRunning = true;
// Server character name in the INI, when changed a character changed
char ServerCharacterINI[MAX_STRING] = { 0 };
class MQ2ElixirType* pElixirType = nullptr;

Elixir* pElixir = nullptr;

class MQ2ElixirType : public MQ2Type {
private:
	char lastAction[MAX_STRING];
	char gem1[MAX_STRING];
	char gem2[MAX_STRING];
	char gem3[MAX_STRING];
	char gem4[MAX_STRING];
	char gem5[MAX_STRING];
	char gem6[MAX_STRING];
	char gem7[MAX_STRING];
	char gem8[MAX_STRING];
	char gem9[MAX_STRING];
	char gem10[MAX_STRING];
	char gem11[MAX_STRING];
	char gem12[MAX_STRING];
	char gem13[MAX_STRING];
	char button1[MAX_STRING];
	char button2[MAX_STRING];
	char button3[MAX_STRING];
	char button4[MAX_STRING];
	char button5[MAX_STRING];
	char button6[MAX_STRING];
	char button7[MAX_STRING];
	char button8[MAX_STRING];
	char button9[MAX_STRING];
	char button10[MAX_STRING];
	char button11[MAX_STRING];
	char button12[MAX_STRING];
public:
	enum ElixirMembers {
		Stance,
		IsFacingTarget,
		TargetHasBuff,
		SpawnIDHasBuff,
		LastAction,
		Gem1,
		Gem2,
		Gem3,
		Gem4,
		Gem5,
		Gem6,
		Gem7,
		Gem8,
		Gem9,
		Gem10,
		Gem11,
		Gem12,
		Gem13,
		Button1,
		Button2,
		Button3,
		Button4,
		Button5,
		Button6,
		Button7,
		Button8,
		Button9,
		Button10,
		Button11,
		Button12,
		IsEnabled,
		IsHateAIEnabled,
	};
	enum ElixirMethods {
		Cast,
		CastItem,
		CastSpell,
		CastAbility,
		CastCombatAbility,
		CastAA,
		Memorize,
		Enable,
		Disable,
		HateAIDisable,
		HateAIEnable,
	};
	bool GetMember(MQ2VARPTR VarPtr, char* Member, char* Index, MQ2TYPEVAR& Dest);
	MQ2ElixirType() : MQ2Type("Elixir") {
		TypeMember(Stance);
		TypeMember(IsFacingTarget);
		TypeMember(TargetHasBuff);
		TypeMember(SpawnIDHasBuff);
		TypeMember(LastAction);
		TypeMember(Gem1);
		TypeMember(Gem2);
		TypeMember(Gem3);
		TypeMember(Gem4);
		TypeMember(Gem5);
		TypeMember(Gem6);
		TypeMember(Gem7);
		TypeMember(Gem8);
		TypeMember(Gem9);
		TypeMember(Gem10);
		TypeMember(Gem11);
		TypeMember(Gem12);
		TypeMember(Gem13);
		TypeMember(Button1);
		TypeMember(Button2);
		TypeMember(Button3);
		TypeMember(Button4);
		TypeMember(Button5);
		TypeMember(Button6);
		TypeMember(Button7);
		TypeMember(Button8);
		TypeMember(Button9);
		TypeMember(Button10);
		TypeMember(Button11);
		TypeMember(Button12);
		TypeMember(IsEnabled);
		TypeMember(IsHateAIEnabled);
		TypeMethod(Cast);
		TypeMethod(CastItem);
		TypeMethod(CastSpell);
		TypeMethod(CastAbility);
		TypeMethod(CastCombatAbility);
		TypeMethod(CastAA);
		TypeMethod(Memorize);
		TypeMethod(Enable);
		TypeMethod(Disable);
		TypeMethod(HateAIEnable);
		TypeMethod(HateAIDisable);
	}
	bool ToString(MQ2VARPTR VarPtr, PCHAR Destination) {
		strcpy_s(Destination, MAX_STRING, "TRUE");
		return true;
	}
	bool FromData(MQ2VARPTR& VarPtr, MQ2TYPEVAR& Source) {
		return false;
	}
	bool FromString(MQ2VARPTR& VarPtr, PCHAR Source) {
		return false;
	}
};

BOOL dataElixir(PCHAR szName, MQ2TYPEVAR& Dest);
PLUGIN_API void ElixirCommand(PSPAWNINFO pLPlayer, char* szLine);
bool WINAPI WritePrivateProfileInt(LPCSTR AppName, LPCSTR KeyName, INT Value, LPCSTR Filename);
void SaveINI();
void LoadINI();