#include "utils.h"

SOCKET sock = INVALID_SOCKET;

int get_string(char *buffer, int max_len) {
	if (fgets(buffer, max_len, stdin) == NULL) {
		return INPUT_CANCELED;
	}

	if (strchr(buffer, '\n') == NULL) {
		int c;
		while ((c = getchar()) != '\n' && c != EOF);
		return INPUT_TOO_LONG;
	}

	buffer[strcspn(buffer, "\n")] = '\0';
	return INPUT_SUCCESS;
}

int get_custom_ip(char *ip) {
	while (1) {
		printf("\nEnter IP Address: ");

		int ip_status = get_string(ip, IP_LEN);

		if (ip_status == INPUT_CANCELED) {
			printf("\nInput canceled. Exiting...\n");
			return INPUT_CANCELED;
		}
		else if (ip_status == INPUT_TOO_LONG) {
			printf("\nError: Input is too long for an IP address! Please try again.\n");
			continue;
		}

		struct in_addr sa;
		if (inet_pton(AF_INET, ip, &sa) == 1) {
			return INPUT_SUCCESS;
		}
		else {
			printf("\nError: Invalid IP address format! Please try again.");
		}
	}
}

int get_custom_port(u_short *port) {
	char port_buffer[PORT_BUFFER_LEN];
	while (1) {
		printf("Enter Port (1-65535): ");

		int port_status = get_string(port_buffer, sizeof(port_buffer));

		if (port_status == INPUT_CANCELED) {
			printf("\nInput canceled. Exiting...\n");
			return INPUT_CANCELED;
		}
		else if (port_status == INPUT_TOO_LONG) {
			printf("\nError: Input is too long for a port number! Please try again.\n");
			continue;
		}

		char *endptr;
		long val = strtol(port_buffer, &endptr, 10);

		if (endptr != port_buffer && *endptr == '\0' && (val >= 1 && val <= 65535)) {
			*port = (u_short)val;
			return INPUT_SUCCESS;
		}
		else {
			printf("\nError: Invalid port number! Must be an integer between 1 and 65535.\n");
		}
	}
}

int get_server_config(char *ip_buffer, u_short *port) {

	char input_buffer[INPUT_BUFFER_LEN];
	int valid = 0;

	while (!valid) {
		printf("Press Enter to continue with default IP and port (127.0.0.1:8080), or type 'custom' to enter manually: ");
		int input_ret = get_string(input_buffer, sizeof(input_buffer));

		if (input_ret == INPUT_CANCELED) {
			printf("\nInput canceled. Exiting...\n");
			return 0;
		}
		else if (input_ret == INPUT_TOO_LONG) {
			printf("\nError: Input is too long! Please try again.\n\n");
			continue;
		}

		if (input_buffer[0] == '\0') {
			valid = 1;
		}
		else if (strcmp(input_buffer, "custom") == 0) {
			if (get_custom_ip(ip_buffer) == INPUT_CANCELED) return 0;
			if (get_custom_port(port) == INPUT_CANCELED) return 0;
			valid = 1;
		}
		else {
			printf("Error: Unknown command. Please press Enter or type 'custom'.\n\n");
		}
	}
	return 1;
}

SOCKET connect_to_server(const char *ip, u_short port) {
	WSADATA wsaData;
	struct sockaddr_in serv_addr;
	SOCKET new_sock = INVALID_SOCKET;

	wprintf(L"\nInitializing Winsock...\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		wprintf(L"Error: WSAStartup failed. Error Code: %d\n", WSAGetLastError());
		return INVALID_SOCKET;
	}

	if ((new_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		wprintf(L"Error: Could not create socket. Error Code: %d\n", WSAGetLastError());
		WSACleanup();
		return INVALID_SOCKET;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
		wprintf(L"Error: Invalid or unsupported address.\n");
		closesocket(new_sock);
		WSACleanup();
		return INVALID_SOCKET;
	}

	wprintf(L"Connecting to the server... (%hs:%d)\n", ip, port);
	if (connect(new_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
		wprintf(L"Error: Connection failed.\n");
		closesocket(new_sock);
		WSACleanup();
		return INVALID_SOCKET;
	}

	wprintf(L"Connection successful!\n\n");
	return new_sock;
}

void run_message_loop() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}