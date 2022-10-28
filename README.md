# 简单的服务器

使用了 IO多路复用模型的 select

epoll > poll > select

## 编译运行命令

`rm -f simple-server && make && ./simple-server`

## 服务端日志

```
# rm -f simple-server && make && ./simple-server 90
gcc -o simple-server main.c server.c client.c util.c
server is running
===================================================
all readfds: 3 
all writefds: 
(1) accept a client, new client is 4
===================================================
all readfds: 3 4 
all writefds: 
(1) accept a client, new client is 5
===================================================
all readfds: 3 4 5 
all writefds: 
(1) read from client 4
===================================================
all readfds: 3 5 
all writefds: 4 
(1) write to client 4
===================================================
all readfds: 3 5 
all writefds: 
(1) read from client 5
===================================================
all readfds: 3 
all writefds: 5 
(1) write to client 5
===================================================
all readfds: 3 
all writefds: 
...
```

## 客户端日志

客户端1

```
# telnet localhost 90
Trying ::1...
telnet: connect to address ::1: Connection refused
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
123
234
Connection closed by foreign host.
```

客户端2

```
telnet localhost 90
Trying ::1...
telnet: connect to address ::1: Connection refused
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
abc
bcd
Connection closed by foreign host.
```

## JMeter简单压测

待办