
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#define PORT 2345
#define MAXSIZE 1024



int main(int argc, char *argv[])
{
    int sockfd, newsockfd;
    //定义服务端套接口数据结构
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int sin_zise, portnumber;
    //发送数据缓冲区
    char buf[MAXSIZE];
    //定义客户端套接口数据结构
    int addr_len = sizeof(struct sockaddr_in);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "create socket failed\n");
        exit(EXIT_FAILURE);
    }
    puts("create socket success");
    printf("sockfd is %d\n", sockfd);
    //清空表示地址的结构体变量
    bzero(&server_addr, sizeof(struct sockaddr_in));
    //设置addr的成员变量信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    //设置ip为本机IP
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) < 0)
    {
        fprintf(stderr, "bind failed \n");
        exit(EXIT_FAILURE);
    }
    puts("bind success\n");
    if (listen(sockfd, 10) < 0)
    {
        perror("listen fail\n");
        exit(EXIT_FAILURE);
    }
    puts("listen success\n");
    int sin_size = sizeof(struct sockaddr_in);
    printf("sin_size is %d\n", sin_size);
    if ((newsockfd = accept(sockfd, (struct sockaddr *)(&client_addr), &sin_size)) < 0)
    {
        perror("accept error");
        exit(EXIT_FAILURE);
    }
    printf("accepted a new connetction\n");
    printf("new socket id is %d\n", newsockfd);
    printf("Accept clent ip is %s\n", inet_ntoa(client_addr.sin_addr));
    printf("Connect successful please input message\n");
    char sendbuf[1024];
    char mybuf[1024];
    while (1)
    {
        int len = recv(newsockfd, buf, sizeof(buf), 0);
        if (strcmp(buf, "exit\n") == 0)
            break;
        fputs(buf, stdout);
        send(newsockfd, buf, len, 0);
        memset(sendbuf, 0 ,sizeof(sendbuf));
        memset(buf, 0, sizeof(buf));
    }
    close(newsockfd);
    close(sockfd);
    puts("exit success");
    exit(EXIT_SUCCESS);
    return 0;
}

