#include "utils.h"

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
