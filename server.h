#ifndef __SERVER_H__
#define __SERVER_H__

#include "client.h"

typedef struct {
	int fd;
	int port;
} _server;

void init_server(_server *s);
int accept_client(_server *s);
void process_client_query(char *output, char *input);

#endif