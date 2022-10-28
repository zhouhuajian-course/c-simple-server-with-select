#include <string.h>
#include <stdio.h>
#include "client.h"

client client_create(int fd) {
	client c;
	c.fd = fd;
	return c;
}

void client_input(client *c) {
	char buf[8] = {'\0'};
	read(c->fd, buf, sizeof(buf));
	// 去掉末尾的\r\n如果是
	int i = 0;
	memset(c->inbuf, '\0', sizeof(c->inbuf))
    for (; i < 8; i++) {
        if (buf[i] == '\r' || buf[i] == '\n' || buf[i] == '\0') {
            break;
        }
        c->outbuf[i] = buf[i];
    }
    c->outbuf[i] = '\0';
}

void client_output(client *c) {
	write(c->fd, c->outbuf, 8);
	write(c->fd, "\n", 1);
}

void client_close(client *c) {
	close(c->fd);
}