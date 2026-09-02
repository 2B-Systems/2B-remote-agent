#include "utils.h"

SOCKET sock = INVALID_SOCKET;

int get_string(wchar_t *buffer, int max_len) {
	if (fgetws(buffer, max_len, stdin) == NULL) {
		return INPUT_CANCELED;
	}

	if (wcsrchr(buffer, L'\n') == NULL) {
		wint_t c;
		while ((c = getwchar()) != L'\n' && c != WEOF);
		return INPUT_TOO_LONG;
	}

	buffer[wcscspn(buffer, L"\n")] = L'\0';
	return INPUT_SUCCESS;
}

int get_custom_ip(wchar_t *ip) {
	while (1) {
		wprintf(L"\nEnter IP Address: ");

		int ip_status = get_string(ip, IP_LEN);

		if (ip_status == INPUT_CANCELED) {
			wprintf(L"\nInput canceled. Exiting...\n");
			return INPUT_CANCELED;
		}
		else if (ip_status == INPUT_TOO_LONG) {
			wprintf(L"\nError: Input is too long for an IP address! Please try again.\n");
			continue;
		}

		struct in_addr sa;
		if (InetPtonW(AF_INET, ip, &sa) == 1) {
			return INPUT_SUCCESS;
		}
		else {
			wprintf(L"\nError: Invalid IP address format! Please try again.");
		}
	}
}

int get_custom_port(u_short *port) {
	wchar_t port_buffer[PORT_BUFFER_LEN];
	while (1) {
		wprintf(L"Enter Port (1-65535): ");

		int port_status = get_string(port_buffer, _countof(port_buffer));

		if (port_status == INPUT_CANCELED) {
			wprintf(L"\nInput canceled. Exiting...\n");
			return INPUT_CANCELED;
		}
		else if (port_status == INPUT_TOO_LONG) {
			wprintf(L"\nError: Input is too long for a port number! Please try again.\n");
			continue;
		}

		wchar_t *endptr;
		long val = wcstol(port_buffer, &endptr, 10);

		if (endptr != port_buffer && *endptr == L'\0' && (val >= 1 && val <= 65535)) {
			*port = (u_short)val;
			return INPUT_SUCCESS;
		}
		else {
			wprintf(L"\nError: Invalid port number! Must be an integer between 1 and 65535.\n");
		}
	}
}

int get_server_config(wchar_t *ip_buffer, u_short *port) {
	wchar_t input_buffer[INPUT_BUFFER_LEN];
	int valid = 0;

	while (!valid) {
		wprintf(L"Press Enter to continue with default IP and port (127.0.0.1:8080), or type 'custom' to enter manually: ");
		int input_ret = get_string(input_buffer, _countof(input_buffer));

		if (input_ret == INPUT_CANCELED) {
			wprintf(L"\nInput canceled. Exiting...\n");
			return EXIT_FAILURE;
		}
		else if (input_ret == INPUT_TOO_LONG) {
			wprintf(L"\nError: Input is too long! Please try again.\n\n");
			continue;
		}

		if (input_buffer[0] == L'\0') {
			valid = 1;
		}
		else if (wcscmp(input_buffer, L"custom") == 0) {
			if (get_custom_ip(ip_buffer) == INPUT_CANCELED) return EXIT_FAILURE;
			if (get_custom_port(port) == INPUT_CANCELED) return EXIT_FAILURE;
			valid = 1;
		}
		else {
			wprintf(L"Error: Unknown command. Please press Enter or type 'custom'.\n\n");
		}
	}
	return EXIT_SUCCESS;
}

SOCKET connect_to_server(const wchar_t *ip, u_short port) {
	struct sockaddr_in serv_addr;
	SOCKET new_sock = INVALID_SOCKET;

	if ((new_sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		wprintf(L"Error: Could not create socket. Error Code: %d\n", WSAGetLastError());
		return INVALID_SOCKET;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (InetPtonW(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
		wprintf(L"Error: Invalid or unsupported address.\n");
		closesocket(new_sock);
		return INVALID_SOCKET;
	}

	wprintf(L"Connecting to the server... (%ls:%d)\n", ip, port);

	if (connect(new_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
		wprintf(L"Error: Connection failed.\n");
		closesocket(new_sock);
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
