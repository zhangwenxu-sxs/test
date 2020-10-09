// udp_file_server.c
 
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <memory.h> 
#include <stdlib.h> 
 
#define BUFFER_SIZE 1024
 
int main()
{
    int sockfd,n;
    struct sockaddr_in server,client;
	int addrlen=sizeof(struct sockaddr);
    char filename[100];
    char filepath[100];
 
    char *buffer;//file buffer
    int fileTrans;
 
    buffer = (char *)malloc(sizeof(char)*BUFFER_SIZE);
    printf("init bufferSize=%d  SIZE=%d\n",sizeof(buffer),BUFFER_SIZE);
    bzero(buffer,BUFFER_SIZE); 
    int lenfilepath;
    FILE *fp;
    int writelength;
 
    if((sockfd = socket(AF_INET,SOCK_DGRAM,0))<0)
    {
        printf("socket build error!\n");
    }
    else
    {
        printf("socket build success!\n");
    }
    memset(&server,0,sizeof(server));  //清空server结构体
    server.sin_family= AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(8888);
 
    if((bind(sockfd,(struct sockaddr*)&server,sizeof(server)))==-1)
    {
        printf("bind error!\n");
    }
    else
    {
        printf("bind success!\n");
    }
    
    while(1)
    {
	    printf("waiting....\n");
        memset(filename,'\0',sizeof(filename));
        memset(filepath,'\0',sizeof(filepath));
        lenfilepath = recvfrom(sockfd,filepath,100,0,(struct sockaddr *)&client,&addrlen);
        printf("filepath :%s\n",filepath);
        if(lenfilepath<0)
        {
            printf("recv error!\n");
            return -1;
        }
        else
        {
            int i=0,k=0;  
            for(i=strlen(filepath);i>=0;i--)  
            {  
                if(filepath[i]!='/')      
                {  
                    k++;  
                }  
                else   
                    break;    
            }  
            strcpy(filename,filepath+(strlen(filepath)-k)+1);   
        }
        printf("filename :%s\n",filename);
        fp = fopen(filename,"w");
        if(fp!=NULL)
        {
            int times = 1;
            while(fileTrans =recvfrom(sockfd,buffer,BUFFER_SIZE,0,(struct sockaddr *)&client,&addrlen))
            {
                printf("times = %d   ",times);
                times++;
                if(fileTrans<0)
                {
                    printf("recv2 error!\n");
                    break;
                }

                writelength = fwrite(buffer,sizeof(char),fileTrans,fp);

                if(fileTrans < BUFFER_SIZE)
                {
                    printf("finish writing!\n");
                    break;
                }else{
                    //printf("write succ! %d fileTrans=%d\n",writelength,fileTrans);
                    printf("write successful!\n");
                    //break;
                }
                printf("continue\n");
                bzero(buffer,BUFFER_SIZE); 
            }
            printf("recv finished!\n");
            fclose(fp);
        }
        else
        {
            printf("filename is null!\n");

        }
    }
    close(sockfd);
    return 0;
}
