#pragma once
#include "include.h"

// Prototypes
int send_data_to_server(SOCKET target_sock, const wchar_t *label, const wchar_t *value);
int send_username_to_server(SOCKET target_sock);
int send_fqdn_to_server(SOCKET target_sock);
int send_os_version_to_server(SOCKET target_sock);
int send_uptime_to_server(SOCKET target_sock);
int send_boot_time_to_server(SOCKET target_sock);
