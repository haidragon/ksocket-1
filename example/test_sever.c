#include <stdio.h>
#include "../ksock.h"

#define SERVER_PORT     6004
#define SERVER_ADDR     "192.168.44.132"

char recv_buf[100];

int main(int argc, char const *argv[])
{
    //第一步：先socket打开文件描述符
    int hd = -1;
    int ret = -1;
    struct ksock_init i;
    i.af = KSOCK_INET;
    i.proto = KSOCK_TCP;
    hd = k_socket(i);
    if (KSOCK_ERR == hd)
    {
        k_perror("k_socket");
        return -1;
    }
    printf("sock hd = %d\n", hd);

    //第二步：bind绑定sockfd和当前电脑的ip地址和端口号
    ret = k_listen(hd, SERVER_ADDR, SERVER_PORT, KSOCK_INET);
    if (KSOCK_ERR == ret)
    {
        k_perror("k_listen");
        return -1;
    }
    ret = k_accept(hd);
    if (KSOCK_ERR == ret)
    {
        k_perror("k_accept");
        return -1;
    }

    struct ksock_accept_node *node = NULL;
    while (1)
    {
        ret = k_get_accept_node(hd, node);
        if (ret == KSOCK_SUC)
        {
            //收到连接，可以做想做的事情了
            printf("get accept!!!!!!");
        }
        else
        {
            k_perror("get accept node");
        }
        sleep(1);
    }

    return 0;
}
