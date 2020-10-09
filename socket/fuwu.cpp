/** \fuwu.cpp
 * 服务端，功能：UDP文字接收，TCP文件发送
 * \author 张文旭 937433802@qq.com
 * 版本历史
 * \par 2019/12/26
 * 张文旭编写初始版本
*/

#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<iostream>

using namespace std;

#define SEND_BUF_SIZE 100
#define RECV_BUF_SIZE 100
#define LISTEN_PORT 7801
#define MAX_LISTEN_NUM 5
#define RECEIVE_BUF_SIZE 1024

void *tranFile(void *sendSocket);
int tcp_send();
int udp_recv();


/**\struct sock_double
 * 为了在使用tranFile函数时能够传输两个参数而定义的结构体，传输两个是为了在退出时将两个socket都关闭
 * 内容包括两个int类型的指针

*/
struct sock_double
{
	int* sock_1;
	int* sock_2;
};


int main()
{
	while(1)
	{
		printf("请选择需要的功能（1为文字接收，2为文件发送,0为退出）：\n");
		int func = 0;
		scanf("%d",&func);
		if(func == 1)
		{
			udp_recv();
		}
		else if(func == 2)
		{
			printf("等待连接\n");
			tcp_send();
		}
		else if(func == 0)
		{
			printf("程序结束\n");
			break;
		}
		else
		{
			printf("输入错误，请重新输入\n");
		}
	}
}

/**\udp_recv
 * @bref 使用UDP方法接收客户端使用UDP发送的文字
 * 参数列表 无
 * 返回值 无意义
*/
int udp_recv()
{
	//服务器监听的Socker
	int listen_sock = 0;
	
	//本机的socket地址
	struct sockaddr_in hostaddr;
	//客户端的socket地址
	struct sockaddr_in clientaddr;
	
	//接收数据的内存
	char recvbuf[RECV_BUF_SIZE] = {0};
	
	memset((void*)&hostaddr,0,sizeof(hostaddr));
	memset((void*)&clientaddr,0,sizeof(clientaddr));
	
	//设置本机的socket地址
	hostaddr.sin_family = AF_INET;
	hostaddr.sin_port = htons(LISTEN_PORT);
	hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//创建Socket
	listen_sock = socket(AF_INET,SOCK_DGRAM,0);
	if(listen_sock < 0)
	{
		printf("create socket failed\n");
		exit(1);
	}
	
	//绑定Socket
	if(bind(listen_sock,(struct sockaddr*)&hostaddr,sizeof(hostaddr)) < 0)
	{
		printf("bind socket failed\n");
		close(listen_sock);
		exit(1);
	}

	while(1)
	{
		printf("等待接收信息\n");
		int iRealLength = recv(listen_sock,recvbuf,RECV_BUF_SIZE,0);

		if(iRealLength < 0)
		{
			printf("receive data error\n");
			close(listen_sock);
			exit(1);
		}
		if(!strcmp(recvbuf,"q\n"))
		{
			printf("客户端已退出，程序结束\n");
			break;		
		}
		printf("receive data is: %s length:%d\n",recvbuf,iRealLength);
	}

	//关闭监听的socket
	close(listen_sock);
	return 0;
}


/**\tranFile TCP方法传输文件
 * @bref 使用UDP方法接收客户端使用UDP发送的文字
 * 参数列表 自定义结构体 内容为TCP文件传输时用到的socket，以及程序结束时需要关闭的socket
 * 返回值 无
*/
void *tranFile(void *sendSocket)
{
	sock_double sock = *(sock_double*)sendSocket;
	unsigned char buf[1024];
	int buflen = 1024,k = 0;
	char file[256] = {0};
	//sprintf(file,"1.dat");
	
	//输入传输的文件名
	printf("当前目录中的文件：\n");
	system("ls");
	printf("请输入需要传输的文件的名称(输入q退出)：\n");
	scanf("%s",file);
	if(!strcmp(file,"q"))
	{
		int iSendLen = write(*(sock.sock_1),file,strlen(file));
		close(*(sock.sock_1));
		close(*(sock.sock_2));
		exit(0);
	}
	FILE *source;
	source = fopen(file,"rb");
	if(source == NULL)
	{
		printf("open file failed\n");
		close(*(sock.sock_1));
		return 0;
	}
	else
	{
		printf("begin tran file%s\n",file);
		//先把文件名发过来
		int iSendLen = write(*(sock.sock_1),file,strlen(file));
		if(iSendLen < 0)
		{
			printf("send file name error\n");
			fclose(source);
			close(*(sock.sock_1));
			return 0;
		}
		
		//等待对方准备好
		memset(buf,0,buflen);
		int iReadLength = read(*(sock.sock_1),buf,1024);
		printf("get string:%s,%d\n",buf,iReadLength);
		
		//开始发送文件内容
		memset(buf,0,buflen);
		while((k = fread(buf,1,buflen,source)) > 0)
		{
			printf("tran file Length:%d\n",k);
			//循环发送直到，文件结束
			int iSendLen = write(*(sock.sock_1),buf,k);
			if(iSendLen < 0)
			{
				printf("send file error\n");
				break;
			}
			memset(buf,0,buflen);
		}
		
		printf("tran file finish\n");
		fclose(source);
		close(*(sock.sock_1));
	}

	return 0;
}


/**\tcp_send
 * @bref 使用TCP方法向客户端发送文件
 * 参数列表 无
 * 返回值 无意义
*/
int tcp_send()
{
	//服务器监听的Socket
	int listen_sock = 0;
	//与客户端连接的Socket
	int app_sock = 0;
	//本机的socket地址
	struct sockaddr_in hostaddr;
	//客户端的socket地址
	struct sockaddr_in clientaddr;
	//客户端地址数据大小
	int socklen = sizeof(clientaddr);
	
	memset((void*)&hostaddr,0,sizeof(hostaddr));
	memset((void*)&clientaddr,0,sizeof(clientaddr));

	//设置本机的socket地址
	hostaddr.sin_family = AF_INET;
	hostaddr.sin_port = htons(LISTEN_PORT);
	hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//创建Socket
	listen_sock = socket(AF_INET,SOCK_STREAM,0);
	if(listen_sock < 0)
	{
		printf("create socket failed\n");
		exit(1);
	}
	
	//避开TIME_WAIT状态，防止socket bind failed
	int on = 1;
	int s = setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	if(s < 0)
	{
		perror("setsockopt");
		exit(6);
	}

	//绑定Socket
	if(bind(listen_sock,(struct sockaddr*)&hostaddr,sizeof(hostaddr)) < 0)
	{
		printf("bind socket failed\n");
		close(listen_sock);
		exit(1);
	}

	//开始监听客户端请求
	if(listen(listen_sock,MAX_LISTEN_NUM) < 0)
	{
		printf("listen failed\n");
		close(listen_sock);
		exit(1);
	}

	//线程定义
	pthread_t tid;

	while(1)
	{
		//接收客户端的连接请求
		app_sock = accept(listen_sock,(struct sockaddr*)&clientaddr,(socklen_t*)&socklen);
		printf("连接成功\n");
		sock_double sock_dbl;
		sock_dbl.sock_1 = &app_sock;
		sock_dbl.sock_2 = &listen_sock;
		
		int ret = pthread_create(&tid,NULL,tranFile,(void*)&sock_dbl);
		if(ret != 0)
		{
			printf("thread create error\n");
			close(app_sock);
			exit(1);
		}
	}

	//关闭监听的socket
	close(listen_sock);
	
	return 0;
}
