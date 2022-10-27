# 简单的服务器

使用了 IO 多路复用的 select

```
[root@centos /tmp]# ./simple-server 80
server port: 80
ready to accept connections
server socket file descriptor: 3
=== waiting ===
file descriptor ready number: 1
read from a socket file descriptor: 3
a client connection is accepted
=== waiting ===
file descriptor ready number: 1
read from a socket file descriptor: 4
received message: a
replied message: b
=== waiting ===
...
```

还有很多Bug