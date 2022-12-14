// author zhouhuajian

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include "config.h"
#include "server.h"
#include "client.h"
#include "util.h"

static _server server;            // 全局服务器
static client clients[1024];      // 1024个客户端
static fd_set readfds;            // 监听可读的Socket文件描述符
static fd_set writefds;           // 监听可写的Socket文件描述符
static fd_set exceptfds;          // 监听出现异常的Socket文件描述符

int main(int argc, char** argv) {
    server.port = SERVER_DEFAULT_PORT;
    if (argc >= 2) server.port = atoi(argv[1]);

    init_server(&server);

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_SET(server.fd, &readfds);

    printf("server is running\n");
    int new_client_fd = -1;
    while (1) {
        printf("===================================================\n");

        fd_set tmpreadfds = readfds;
        fd_set tmpwritefds = writefds;
        fd_set tmpexceptfds = exceptfds;

        print_all_fds(&readfds, "read");
        print_all_fds(&writefds, "write");

        int readynum = select(
            FD_SETSIZE,
            &tmpreadfds, &tmpwritefds, &tmpexceptfds,
            NULL
        );
        if (readynum <= 0) exit(1);
        printf("(%d) ", readynum);
        new_client_fd = -1;
        int fd = 0;
        for (; fd < FD_SETSIZE; fd++) {
            // 有可读的fd
            if (FD_ISSET(fd, &tmpreadfds) && fd != new_client_fd) {
                if (fd == server.fd) {
                    int client_fd = accept_client(&server);
                    clients[client_fd].fd = client_fd;

                    FD_SET(client_fd, &readfds);
                    new_client_fd = client_fd;
                    printf("accept a client, new client is %d\n", client_fd);
                } else {
                    printf("read from client %d\n", fd);
                    client *c_ptr = &clients[fd];
                    // printf("%d %p %p\n", clients[fd].fd, c_ptr, &clients[fd]);
                    client_input(c_ptr);
                    process_client_query(c_ptr->outbuf, c_ptr->inbuf);
                    // printf("%s, %s\n", c_ptr->inbuf, c_ptr->outbuf);
                    // 加入到监听可写的fds
                    FD_CLR(fd, &readfds);
                    FD_SET(fd, &writefds);
                }
            }
            // 有可写的fd
            if (FD_ISSET(fd, &tmpwritefds)) {
                printf("write to client %d\n", fd);
                client *c_ptr = &clients[fd];
                client_output(c_ptr);
                client_close(c_ptr);
                FD_CLR(fd, &writefds);
            }
            // 有出现异常的fd
            if (FD_ISSET(fd, &tmpexceptfds)) {
                // 暂不处理
            }
        }
    }
    return 0;
}