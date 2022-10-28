#ifndef __SERVER_H__
#define __SERVER_H__

typedef struct {
	int fd;
	int port;
} _server;

void init_server(_server *s);
int accept_client(_server *s);
void process_client_query(client *c);

#endif