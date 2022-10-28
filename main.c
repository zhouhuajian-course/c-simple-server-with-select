// author zhouhuajian
#include <stdio.h>
#include "config.h"
#include "server.h"
#include "client.h"

static _server server;            // ȫ�ַ���˶���
static client clients[1024];      // ȫ��1024���ͻ��˶���
static fd_set readfds;            // ȫ�ּ����ɶ���fd
static fd_set writefds;           // ȫ�ּ�����д��fd
static fd_set exceptfds;          // ȫ�ּ������쳣��fd

int main(int argc, char** argv) {
    // ����˿�
    server.port = SERVER_DEFAULT_PORT;
    if (argc >= 2) server.port = atoi(argv[1]);
//    printf("1");
//    exit(0);
    // ��ʼ������
    init_server(&server);
    // ��server socket fd ��ӵ���������fd��
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_SET(server.fd, &readfds);
    printf("server is running\n");
    int new_client_fd;
    while (1) {
        printf("===================================================\n");
        fd_set tmpreadfds = readfds;
        fd_set tmpwritefds = writefds;
        fd_set tmpexceptfds = exceptfds;
        printf("all readfds: ");
        int fd;
        for (fd = 0; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &readfds)) {
                printf("%d ", fd);
            }
        }
        printf("\n");
        printf("all writefds: ");
        for (fd = 0; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &writefds)) {
                printf("%d ", fd);
            }
        }
        printf("\n");
        // printf("select %d %d\n", server.fd, FD_ISSET(server.fd, &tmpreadfds));
        int readynum = select(
            FD_SETSIZE,
            &tmpreadfds, &tmpwritefds, &tmpexceptfds,
            NULL
        );
        if (readynum <= 0) {
            // perror();
            exit(1);
        }
        printf("- %d - ", readynum);
        new_client_fd = -1;
        for (fd = 0; fd < FD_SETSIZE; fd++) {
            // �пɶ���fd
            if (FD_ISSET(fd, &tmpreadfds) && fd != new_client_fd) {
                if (fd == server.fd) {
                    int client_fd = accept_client(&server);
                    client c = client_create(client_fd);
                    clients[c.fd] = c;
                    FD_SET(client_fd, &readfds);
                    new_client_fd = client_fd;
                    printf("accept a client, new client is %d\n", client_fd);
                } else {
                    printf("read from client %d\n", fd);
                    client c = clients[fd];
                    client_input(&c);
                    process_client_query(&c);
                    printf("__%s %s\n", c.inbuf, c.outbuf);
                    FD_CLR(fd, &readfds);
                    FD_SET(fd, &writefds);
                }
            }
            // �п�д��fd
            if (FD_ISSET(fd, &tmpwritefds)) {
                printf("write to client %d\n", fd);
                client c = clients[fd];
                client_output(&c);
                client_close(&c);
                FD_CLR(fd, &writefds);
            }
            // �г����쳣��fd
            if (FD_ISSET(fd, &tmpexceptfds)) {
                // �ݲ������쳣
            }
        }
    }
    return 0;
}