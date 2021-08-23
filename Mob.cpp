#include "Mob.h"
#include "../MQ2Plugin.h"
#include "Core.h"

using namespace std;

int MobHP(PSPAWNINFO pSpawn) {
	int hp = MobHPDannet(pSpawn);
	if (hp > 0) return hp;
	return MobHPEstimate(pSpawn);
}

int MobHPDannet(PSPAWNINFO pSpawn) {
	char szTemp[MAX_STRING];
	if (!pSpawn->Type == SPAWN_PLAYER) return 0;

	PMQPLUGIN pLook = Plugin("mq2dannet");
	if (!pLook) return 0;
	bool(*f_peer_connected)(const std::string & name) = (bool(*)(const std::string & name))GetProcAddress(pLook->hModule, "peer_connected");
	if (!f_peer_connected) return 0;
	
	sprintf_s(szTemp, "%s_%s", EQADDR_SERVERNAME, pSpawn->Name);
	if (!f_peer_connected(szTemp)) return 0;
	sprintf_s(szTemp, "${DanNet[%s].OSet[Me.CurrentHPs]}", pSpawn->Name);
	ParseMacroData(szTemp, sizeof(szTemp));
	if (!_stricmp(szTemp, "FALSE")) {
		sprintf_s(szTemp, "/dobserve %s -q Me.CurrentHPs", pSpawn->Name);
		ParseMacroData(szTemp, sizeof(szTemp));
		return 0;
	}
	sprintf_s(szTemp, "${DanNet[%s].Observe[Me.CurrentHPs]}", pSpawn->Name);
	ParseMacroData(szTemp, sizeof(szTemp));
	return atoi(szTemp);
}

int MobHPEstimate(PSPAWNINFO pSpawn) {
	int hp = 0;
	int level = pSpawn->Level;
	if (GetCharInfo()->zoneId < 200 || level < 48) return level * 20;
	return level * 100;	
}

bool IsPlayerPoisoned(PSPAWNINFO pSpawn) {
	return IsPlayerPoisonedDannet(pSpawn);
}

bool IsPlayerPoisonedDannet(PSPAWNINFO pSpawn) {
	char szTemp[MAX_STRING];
	if (!pSpawn->Type == SPAWN_PLAYER) return false;

	PMQPLUGIN pLook = Plugin("mq2dannet");
	if (!pLook) return 0;
	bool(*f_peer_connected)(const std::string & name) = (bool(*)(const std::string & name))GetProcAddress(pLook->hModule, "peer_connected");
	if (!f_peer_connected) return 0;

	sprintf_s(szTemp, "%s_%s", EQADDR_SERVERNAME, pSpawn->Name);
	if (!f_peer_connected(szTemp)) return 0;

	sprintf_s(szTemp, "${DanNet[%s].OSet[Me.Poisoned]}", pSpawn->Name);	
	ParseMacroData(szTemp, sizeof(szTemp));
	if (!_stricmp(szTemp, "FALSE")) {		
		sprintf_s(szTemp, "/dobserve %s -q Me.Poisoned", pSpawn->Name);
		ParseMacroData(szTemp, sizeof(szTemp));
		return false;
	}
	sprintf_s(szTemp, "${DanNet[%s].Observe[Me.Poisoned]}", pSpawn->Name);
	ParseMacroData(szTemp, sizeof(szTemp));
	return _stricmp(szTemp, "NULL");
}