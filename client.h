#ifndef __CLIENT_H__
#define __CLIENT_H__

typedef struct {
    int fd; // socketFileDescriptor
    char inbuf[8];  // 暂时支持8个字符 除去\0 其实是7个字符
    char outbuf[8]; // 暂时支持8个字符 除去\0 其实是7个字符
} client;

void client_input(client *c_ptr);
void client_output(client *c_ptr);
void client_close(client *c_ptr);

#endif