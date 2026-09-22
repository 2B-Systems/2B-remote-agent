#include "os.h"

void append_username_to_json(cJSON *json) {
    wchar_t userName[UNLEN + 1];
    DWORD unLen = UNLEN + 1;

    if (GetUserNameW(userName, &unLen)) {
        JSON_AddWideString(json, "Username", userName);
        return;
    }

    wchar_t errorLog[128];
    swprintf_s(errorLog, _countof(errorLog), L"Error: GetUserNameW failed. Error Code: %lu\n", GetLastError());
    OutputDebugStringW(errorLog);

    JSON_AddWideString(json, "Username", L"Unknown");
}

void append_fqdn_to_json(cJSON *json) {
    DWORD dwSize = 0;

    if (!GetComputerNameExW(ComputerNameDnsFullyQualified, NULL, &dwSize)) {
        if (GetLastError() == ERROR_MORE_DATA && dwSize > 0) {
            wchar_t *fqdn = (wchar_t *)malloc(dwSize * sizeof(wchar_t));

            if (fqdn != NULL) {
                if (GetComputerNameExW(ComputerNameDnsFullyQualified, fqdn, &dwSize)) {
                    JSON_AddWideString(json, "Hostname/FQDN", fqdn);
                    free(fqdn);
                    return;
                }
                free(fqdn);
            }
        }
    }

    wchar_t errorLog[128];
    swprintf_s(errorLog, _countof(errorLog), L"Error: GetComputerNameExW (FQDN) failed. Error Code: %lu\n", GetLastError());
    OutputDebugStringW(errorLog);

    JSON_AddWideString(json, "Hostname/FQDN", L"Unknown");
}

typedef LONG(WINAPI *RtlGetVersionPtr)(POSVERSIONINFOEXW);

void append_os_version_to_json(cJSON *json) {
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
                JSON_AddWideString(json, "OS", osVersionStr);
                return;
            }
        }
    }

    wchar_t errorLog[128];
    swprintf_s(errorLog, _countof(errorLog), L"Error: RtlGetVersion failed. Error Code: %lu\n", GetLastError());
    OutputDebugStringW(errorLog);

    JSON_AddWideString(json, "OS", L"Unknown");
}

void append_os_architecture_to_json(cJSON *json) {
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

    JSON_AddWideString(json, "OS Architecture", archStr);
}

void append_uptime_to_json(cJSON *json) {
    ULONGLONG uptimeMs = GetTickCount64();

    ULONGLONG seconds = (uptimeMs / 1000) % 60;
    ULONGLONG minutes = (uptimeMs / (1000 * 60)) % 60;
    ULONGLONG hours = (uptimeMs / (1000 * 60 * 60)) % 24;
    ULONGLONG days = uptimeMs / (1000 * 60 * 60 * 24);

    wchar_t uptimeStr[128];
    _snwprintf_s(uptimeStr, _countof(uptimeStr), _TRUNCATE,
        L"%llu Days, %llu Hours, %llu Minutes, %llu Seconds",
        days, hours, minutes, seconds);

    JSON_AddWideString(json, "Uptime", uptimeStr);
}

void append_boot_time_to_json(cJSON *json) {
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

    JSON_AddWideString(json, "Boot Time", bootTimeStr);
}