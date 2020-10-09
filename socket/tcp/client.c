#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#define PORT 2345
int count = 1;
int main()
{
    int sockfd;
    char buffer[2014];
    struct sockaddr_in server_addr;
    struct hostent *host;
    int nbytes;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Socket Error is %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //客户端发出请求
    if (connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Connect failed\n");
        exit(EXIT_FAILURE);
    }
    char sendbuf[1024];
    char recvbuf[2014];
    while (1)
        {
        fgets(sendbuf, sizeof(sendbuf), stdin);
        send(sockfd, sendbuf, strlen(sendbuf), 0);
        if (strcmp(sendbuf, "exit\n") == 0)
            break;
        recv(sockfd, recvbuf, sizeof(recvbuf), 0);
        fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }
    close(sockfd);
    exit(EXIT_SUCCESS);
    return 0;
}

