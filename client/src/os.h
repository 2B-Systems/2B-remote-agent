#pragma once
#include "include.h"

// Prototypes
char *WideToUTF8(const wchar_t *wstr);
int send_data_to_server(SOCKET target_sock, const wchar_t *label, const wchar_t *value);
void append_username_to_json(cJSON *json);
void append_fqdn_to_json(cJSON *json);
void append_os_version_to_json(cJSON *json);
void append_os_architecture_to_json(cJSON *json);
void append_uptime_to_json(cJSON *json);
void append_boot_time_to_json(cJSON *json);