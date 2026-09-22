#pragma once

#define WIN32_LEAN_AND_MEAN

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

// Custom Headers
#include "cJSON.h"
#include "utils.h"
#include "info-sender.h"
#include "keyboard.h"
#include "os.h"