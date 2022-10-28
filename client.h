#ifndef __CLIENT_H__
#define __CLIENT_H__


// �ͻ���

typedef struct {
    int fd;    // socketFileDescriptor
    char inbuf[8]; // ʵ����ֻ��Ҫ��һ���ַ�
    char outbuf[8];    // ʵ����ֻ���͵�һ���ַ�
} client;

client client_create(int fd);
void client_input(client *c);
void client_output(client *c);
void client_close(client *c);


#endif