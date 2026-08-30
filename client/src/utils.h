#pragma once
#include "include.h"

#define IP_LEN 20 
#define PORT_BUFFER_LEN 12
#define INPUT_BUFFER_LEN 100

typedef enum {
	INPUT_CANCELED = -1,
	INPUT_SUCCESS = 0,
	INPUT_TOO_LONG = 1
} InputStatus;

InputStatus get_string(char *buffer, int max_len);
int get_custom_ip(char *ip);
int get_custom_port(u_short *port);
