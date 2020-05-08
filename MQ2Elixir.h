#pragma once

#include "OptionsWnd.h"
#include "LaunchWnd.h"
#include "BaseElixir.h"

unsigned long PulseDelay;
bool IsZoning;
bool IsWrongUI;
bool IsElixirLaunchWndActive;
bool IsElixirOptionsWndActive;
CElixirLaunchWnd* ElixirLaunchWnd = NULL;
CElixirOptionsWnd* ElixirOptionsWnd = NULL;

void DestroyElixirWindows();
void CreateElixirWindows();
void CheckWndActive();

BaseElixir* elixir;
