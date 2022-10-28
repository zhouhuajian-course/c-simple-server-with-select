#include <string.h>
#include <stdio.h>
#include "client.h"

client client_create(int fd) {
	client c;
	c.fd = fd;
	return c;
}

void client_input(client *c) {
	char buf[8];
	read(c->fd, c->inbuf, 8);
	// 去掉末尾的\r\n如果是
	int last_i = strlen(c->inbuf) - 1;
    while (c->inbuf[last_i] == '\r' || c->inbuf[last_i] == '\n') {
        c->inbuf[last_i] = '\0';
        last_i--;
    }
}

void client_output(client *c) {
	write(c->fd, c->outbuf, 8);
}

void client_close(client *c) {
	close(c->fd);
}