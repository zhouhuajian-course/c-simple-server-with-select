#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/select.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "client.h"
#include "util.h"

typedef struct {
	int fd;
	int port;
} _server;

void init_server(_server *s);
// ���ؿͻ��� fd
int accept_client(_server *s);
// ����ͻ�������
void process_client_query(client *c);