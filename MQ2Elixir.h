#pragma once

#include "Elixir.h"

unsigned long PulseDelay;
bool IsZoning;
class MQ2ElixirType* pElixirType = nullptr;

void CheckElixir();

Elixir* pElixir = nullptr;

class MQ2ElixirType : public MQ2Type {
private:
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
public:
	enum ElixirMembers {
		Stance,
		IsFacingTarget,
		TargetHasBuff,
		SpawnIDHasBuff,
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
	};
	enum ElixirMethods {
		Cast,
		CastItem,
		CastSpell,
		CastAbility,
		CastCombatAbility,
		CastAA,
		Memorize,
	};
	bool GetMember(MQ2VARPTR VarPtr, char* Member, char* Index, MQ2TYPEVAR& Dest);
	MQ2ElixirType() : MQ2Type("Elixir") {
		TypeMember(Stance);
		TypeMember(IsFacingTarget);
		TypeMember(TargetHasBuff);
		TypeMember(SpawnIDHasBuff);
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

		TypeMethod(Cast);
		TypeMethod(CastItem);
		TypeMethod(CastSpell);
		TypeMethod(CastAbility);
		TypeMethod(CastCombatAbility);
		TypeMethod(CastAA);
		TypeMethod(Memorize);
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