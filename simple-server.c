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

static serverPort = 80;

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
    listen(serverSocketFileDescriptor, 20);
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

    // 向客户端发送数据
    //    char str[] = "test";
    //    write(clientSocketFileDescriptor, str, sizeof(str));

    //关闭套接字
//    close(clientSocketFileDescriptor);
//    close(serverSocketFileDescriptor);
//    return 0;
}

int main(int argc, char *argv[]){
    // 端口
    if (argc >= 2) {
        serverPort = atoi(argv[2]);
    }
    printf("server port: %d\n", serverPort);
    // 第一个 创建服务端socket
    int serverSocketFileDescriptor = createServerSocket();
    return 0;
}

