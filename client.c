#include <string.h>
#include <stdio.h>
#include "client.h"

void client_input(client *c_ptr) {
	char buf[8] = {'\0'};
	read(c_ptr->fd, buf, sizeof(buf));
	// 去掉末尾的\r\n如果是
	int i = 0;
	memset(c_ptr->inbuf, '\0', sizeof(c_ptr->inbuf));
    for (; i < 8; i++) {
        if (buf[i] == '\r' || buf[i] == '\n' || buf[i] == '\0') {
            break;
        }
        c_ptr->inbuf[i] = buf[i];
    }
    c_ptr->inbuf[i] = '\0';
}

void client_output(client *c_ptr) {
	write(c_ptr->fd, c_ptr->outbuf, 8);
	write(c_ptr->fd, "\n", 1);
}

void client_close(client *c_ptr) {
	close(c_ptr->fd);
}