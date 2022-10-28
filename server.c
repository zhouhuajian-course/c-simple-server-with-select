
#include "server.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

void process_client_query(char *output, char *input) {
    // 简单的把客户端发过来的字符进行ASCII+1
    // 假设做了很多处理操作
    int i = 0;
    while (input[i] != '\0') {
        output[i] = input[i] + 1;
        i++;
    }
    output[i] = '\0';
}
