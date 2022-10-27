/*************************************
Description:
简单的服务器 客户端发送一个字符 服务端返回字符ASCII码+1后的字符
使用IO多路复用的select
Author: zhouhuajian
Version: v1.0
**************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* According to POSIX.1-2001 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static serverPort = 80;

#define MESSAGE_REPLY_TYPE_NORMAL 1
#define MESSAGE_REPLY_TYPE_ERROR 2
typedef struct {
    int socketFileDescriptor;
    char receivedMessageBuffer[1024];
    char replyMessageBuffer[1024];
} client;

// 去掉字符串末尾的\r\n
// 012 3 4
// abc\r\n\0
// strlen = 5
//void simpleRTrim(char *charArray) {
//    while (1) {
//        int lastCharIndex = strlen(charArray) - 1;
//        if (charArray[lastCharIndex] != '\r'
//            || charArray[lastCharIndex] != '\n'
//            || charArray[lastCharIndex] != ' ') {
//            break;
//        }
//        charArray[lastCharIndex] = '\0';
//    }
//    printf("%s\n", charArray);
//}
char *trimString(char *str)
{
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0)
        return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

// 创建服务端socket并返回socket描述符
int createServerSocket() {
    // 创建服务端套接字
    int serverSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // 将套接字和IP、端口绑定
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    // serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // serverAddress.sin_port = htons(80);
    serverAddress.sin_port = htons(serverPort);
    bind(serverSocketFileDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    // 进入监听状态，等待用户发起请求
    // int listen(int sockfd, int backlog);
    int backlog = 20;
    listen(serverSocketFileDescriptor, backlog);
    return serverSocketFileDescriptor;
    /*
    //接收客户端请求
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    while (1) {
        int clientSocketFileDescriptor = accept(serverSocketFileDescriptor, (struct sockaddr*)&clientAddress, &clientAddressSize);
        char receivedMessageBuffer[1024];
        memset(receivedMessageBuffer, 0, sizeof(receivedMessageBuffer));
        // ssize_t recv(int socket, void *buffer, size_t length, int flags);
        recv(clientSocketFileDescriptor, receivedMessageBuffer, 1024, MSG_PEEK);
        // 去掉末尾的\r\n和空格
        // simpleRTrim(receivedMessageBuffer);
        // printf("received message length: %d\n", strlen(receivedMessageBuffer));
        // trimString(receivedMessageBuffer);
        printf("received message: %s\n", trimString(receivedMessageBuffer));
        // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
        char *sentMessageBuffer[1024];
        sentMessageBuffer[0] = receivedMessageBuffer[0] + 1;
        send(clientSocketFileDescriptor, sentMessageBuffer, 1, MSG_CONFIRM);
        printf("sent message: %s\n", sentMessageBuffer);
        close(clientSocketFileDescriptor);
    }
    */

    // 向客户端发送数据
    //    char str[] = "test";
    //    write(clientSocketFileDescriptor, str, sizeof(str));

    //关闭套接字
//    close(clientSocketFileDescriptor);
//    close(serverSocketFileDescriptor);
//    return 0;
}

static int serverSocketFileDescriptor;
// 所有文件描述符数量
static int allFileDescriptorNumber = 0;
// 所有文件描述符
static int allFileDescriptorArray[1024];
static int allWriteFileDescriptorArray[1024];
// 所有客户端对用的客户端实例，可以优化
// 客户端ID就是客户端的socket文件描述符
static client allClient[65536];


void addElementToAllWriteFileDescriptorArray(int element) {
    int i;
    for (i = 0; i < 1024; i++) {
        if (allWriteFileDescriptorArray[i] == 0) {
            allWriteFileDescriptorArray[i] = element;
            break;
        }
    }
}

void removeElementFromAllWriteFileDescriptorArray(int element) {
    int i;
    for (i = 0; i < 1024; i++) {
        if (allWriteFileDescriptorArray[i] == element) {
            allWriteFileDescriptorArray[i] = 0;
            break;
        }
    }
}


void addElementToAllFileDescriptorArray(int element) {
    int i;
    for (i = 0; i < 1024; i++) {
        if (allFileDescriptorArray[i] == 0) {
            allFileDescriptorNumber++;
            allFileDescriptorArray[i] = element;
            break;
        }
    }
}

void removeElementFromAllFileDescriptorArray(int element) {
    int i;
    for (i = 0; i < 1024; i++) {
        if (allFileDescriptorArray[i] == element) {
            allFileDescriptorNumber--;
            allFileDescriptorArray[i] = 0;
            break;
        }
    }
}

int getMaxFileDescriptor() {
    int maxFileDescriptor = serverSocketFileDescriptor;
    int i;
    for (i = 0; i < 1024; i++) {
        if (allFileDescriptorArray[i] > maxFileDescriptor) {
            maxFileDescriptor = allFileDescriptorArray[i];
        }
    }
    return maxFileDescriptor;
}

void acceptConnection() {
    // 接收客户端请求
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    int clientSocketFileDescriptor = accept(serverSocketFileDescriptor, (struct sockaddr *) &clientAddress,
                                            &clientAddressSize);
    // 把客户端的socket文件描述符添加到要监听的描述符中
    addElementToAllFileDescriptorArray(clientSocketFileDescriptor);

    client aClient;
    allClient[clientSocketFileDescriptor] = aClient;

    printf("a client connection is accepted\n");
}

// 测试
char replyChars[1024];

void readClientMessage(int clientSocketFileDescriptor) {
    client aClient = allClient[clientSocketFileDescriptor];
    memset(aClient.receivedMessageBuffer, 0, sizeof(aClient.receivedMessageBuffer));
    recv(clientSocketFileDescriptor, aClient.receivedMessageBuffer, 1024, MSG_PEEK);
    // char buffer[1024]={"\0"};
    // int len=read(clientSocketFileDescriptor,buffer,1024);
    // printf("len %d\n", len);
    if (strlen(trimString(aClient.receivedMessageBuffer)) > 0) {
        printf("received message: %s\n", trimString(aClient.receivedMessageBuffer));
        aClient.replyMessageBuffer[0] = aClient.receivedMessageBuffer[0] + 1;
        aClient.replyMessageBuffer[1] = '\0';
        addElementToAllWriteFileDescriptorArray(clientSocketFileDescriptor);
        removeElementFromAllFileDescriptorArray(clientSocketFileDescriptor);
        // printf("测试: %s\n", aClient.replyMessageBuffer);
        // exit(0);
        // 测试
        replyChars[clientSocketFileDescriptor] = aClient.receivedMessageBuffer[0] + 1;
        replyToClient(clientSocketFileDescriptor, MESSAGE_REPLY_TYPE_NORMAL);
    }
    // printf("a client message is read\n");
}


void replyToClient(int clientSocketFileDescriptor, int messageReplyType) {
    if (messageReplyType == MESSAGE_REPLY_TYPE_NORMAL) {

        client aClient = allClient[clientSocketFileDescriptor];
        // printf("测试: %s\n", aClient.receivedMessageBuffer);
        // printf("测试: %s\n", aClient.replyMessageBuffer);
        // exit(0);
        char reply[2];
        sprintf(reply, "%c\n", replyChars[clientSocketFileDescriptor]);
        send(clientSocketFileDescriptor, reply, 2, MSG_CONFIRM);
        printf("replied message: %c\n", replyChars[clientSocketFileDescriptor]);
        // send(clientSocketFileDescriptor, aClient.replyMessageBuffer, 1, MSG_CONFIRM);
        // printf("replied message: %s\n", aClient.replyMessageBuffer);
    } else {
        char *errorMessage = "error occurred";
        send(clientSocketFileDescriptor, errorMessage, strlen(errorMessage), MSG_CONFIRM);
        printf("replied message: %s\n", errorMessage);
    }
    close(clientSocketFileDescriptor);
    removeElementFromAllWriteFileDescriptorArray(clientSocketFileDescriptor);
}

int main(int argc, char *argv[]){
    // 端口
    if (argc >= 2) {
        serverPort = atoi(argv[1]);
    }
    printf("server port: %d\n", serverPort);
    // 第一步 创建服务端socket
    serverSocketFileDescriptor = createServerSocket();
    // 第二步 创建一个用来读、写、异常的文件描述符集合
    fd_set fileDescriptorSetForRead;
    fd_set fileDescriptorSetForWrite;
    fd_set fileDescriptorSetForException;
    memset(allFileDescriptorArray, 0, sizeof(allFileDescriptorArray));
    // 把服务端socket文件描述符添加到所有文件描述符数组中
    addElementToAllFileDescriptorArray(serverSocketFileDescriptor);
    printf("ready to accept connections\n");
    printf("server socket file descriptor: %d\n", serverSocketFileDescriptor);


    while (1) {
        FD_ZERO(&fileDescriptorSetForRead);
        FD_ZERO(&fileDescriptorSetForWrite);
        FD_ZERO(&fileDescriptorSetForException);
        // 可以优化 i < allFileDescriptorNumber
        // 需要在remove元素的时候重新排布数组
//            exit(0);
        int i;
        for (i = 0; i < 1024; i++) {
            if (allFileDescriptorArray[i] != 0) {
                FD_SET(allFileDescriptorArray[i], &fileDescriptorSetForRead);
                FD_SET(allFileDescriptorArray[i], &fileDescriptorSetForException);
            }
            if (allWriteFileDescriptorArray[i] != 0) {
                FD_SET(allFileDescriptorArray[i], &fileDescriptorSetForWrite);
            }
        }
        printf("=== waiting ===\n");
        int ready = select(getMaxFileDescriptor() + 1, &fileDescriptorSetForRead, &fileDescriptorSetForWrite, &fileDescriptorSetForException, NULL);
        if (ready <= 0) {
            continue;
        }
        printf("file descriptor ready number: %d\n", ready);
        // 遍历所有读 fileDescriptorSetForRead已被重置
        for (i = 0; i < 1024; i++) {

            // 可读
            if (FD_ISSET(allFileDescriptorArray[i], &fileDescriptorSetForRead)) {
                printf("read from a socket file descriptor: %d\n", allFileDescriptorArray[i]);
                if (allFileDescriptorArray[i] == serverSocketFileDescriptor) {
                    acceptConnection();
                } else {
                    readClientMessage(allFileDescriptorArray[i]);
                }
            }
            // 可写
            if (FD_ISSET(allWriteFileDescriptorArray[i], &fileDescriptorSetForWrite)) {
                printf("write to a socket file descriptor: %d\n", allWriteFileDescriptorArray[i]);

                replyToClient(allWriteFileDescriptorArray[i], MESSAGE_REPLY_TYPE_NORMAL);
            }
            // 出现异常
            if (FD_ISSET(allFileDescriptorArray[i], &fileDescriptorSetForException)) {
                printf("write error to a socket file descriptor: %d\n", allFileDescriptorArray[i]);
                replyToClient(allFileDescriptorArray[i], MESSAGE_REPLY_TYPE_ERROR);
            }
        }
        // 遍历所有写
        // 遍历所有异常
    }

    // int select(int nfds, fd_set *readfds, fd_set *writefds,
    //                  fd_set *exceptfds, struct timeval *timeout);
    // void FD_CLR(int fd, fd_set *set);
    // int  FD_ISSET(int fd, fd_set *set);
    // void FD_SET(int fd, fd_set *set);
    // void FD_ZERO(fd_set *set);


    return 0;
}

