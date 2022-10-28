#include <sys/select.h>
#include <stdio.h>

void print_all_fds(fd_set *fds_ptr, char *type) {
    printf("all %sfds: ", type);
    int fd;
    for (fd = 0; fd < FD_SETSIZE; fd++) {
        if (FD_ISSET(fd, fds_ptr)) {
            printf("%d ", fd);
        }
    }
    printf("\n");
}

char *strupr(char* str)
{
    char *origin = str;
    for (; *str != '\0'; str++)
        *str = toupper(*str);
    return origin;
}

char *strlowr(char* str)
{
    char* origin = str;
    for (; *str != '\0'; str++)
        *str = tolower(*str);
    return origin;
}

int tolower(int c)
{
    if ((c >= 'A') && (c <= 'Z'))
        return c + ('a' - 'A');
    return c;
}

int toupper(int c)
{
    if ((c >= 'a') && (c <= 'z'))
        return c + ('A' - 'a');
    return c;
}