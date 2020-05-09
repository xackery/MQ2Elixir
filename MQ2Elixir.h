#pragma once

#include "OptionsWnd.h"
#include "LaunchWnd.h"
#include "BaseElixir.h"

unsigned long PulseDelay;
bool IsZoning;
bool IsWrongUI;
bool IsElixirLaunchWndActive;
bool IsElixirOptionsWndActive;
CElixirLaunchWnd* ElixirLaunchWnd = nullptr;
CElixirOptionsWnd* ElixirOptionsWnd = nullptr;
class MQ2ElixirType* pElixirType = nullptr;

void DestroyElixirWindows();
void CreateElixirWindows();
void CheckWndActive();
void CheckElixir();

BaseElixir* pElixir = nullptr;

class MQ2ElixirType : public MQ2Type {
public:
	enum ElixirMembers {
		Stance,
		IsFacingTarget,
		TargetHasBuff,
		SpawnIDHasBuff
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
