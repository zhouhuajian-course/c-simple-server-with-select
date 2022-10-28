// 简单的服务器 客户端发送一个字符 
// 简单的把客户端发过来的字母进行大写
// 使用IO多路复用的select

//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <arpa/inet.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//
//#include <sys/select.h>
//
//#include <sys/time.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include "util.h"

#include "server.h"
#include "client.h"

void init_server(_server *s) {
    // serverSocketFileDescriptor
    s->fd = socket(
        AF_INET, SOCK_STREAM, IPPROTO_TCP
    );
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(s->port);
    // printf("server port: %d\n", s->port);
    bind(s->fd, (struct sockaddr*)&addr, sizeof(addr));
    int backlog = 20;
    listen(s->fd, backlog);
}

int accept_client(_server *s) {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);
    return accept(
        s->fd, (struct sockaddr*)&addr, &addr_size
    );
}

void process_client_query(client *c) {
    // 简单的把客户端发过来的字符进行ASCII+1
    char buf[8];
    int i = 0;
    while (c->inbuf[i] != '\0') {
        buf[i] = c->inbuf[i]++;
    }
    buf[i] = '\0';
    // 可优化 假设做了很多处理操作
    memset(c->outbuf, '\0', sizeof(c->outbuf));
    memcpy(c->outbuf, buf, strlen(buf) + 1);
}
