#pragma once

#include "Elixir.h"

unsigned long PulseDelay;
bool IsZoning;
// Server character name in the INI, when changed a character changed
char ServerCharacterINI[MAX_STRING] = { 0 };
Elixir* pElixir = nullptr;

PLUGIN_API void ElixirCommand(PSPAWNINFO pLPlayer, char* szLine);
bool WINAPI WritePrivateProfileInt(LPCSTR AppName, LPCSTR KeyName, INT Value, LPCSTR Filename);
void SaveINI();
void LoadINI();
void DrawElixirUIPanel();
void DrawElixirUI();