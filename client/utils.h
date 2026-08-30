#pragma once
#include "include.h"

// Global Variables

extern SOCKET sock;

// Macros

#define IP_LEN 20 
#define PORT_BUFFER_LEN 12
#define INPUT_BUFFER_LEN 100

// Data Structures

typedef enum {
	INPUT_CANCELED = -1,
	INPUT_SUCCESS = 0,
	INPUT_TOO_LONG = 1
} InputStatus;

// Prototypes

InputStatus get_string(char *buffer, int max_len);
int get_custom_ip(char *ip);
int get_custom_port(u_short *port);
int get_server_config(char *ip_buffer, u_short *port);
SOCKET connect_to_server(const char *ip, u_short port);
void run_message_loop();