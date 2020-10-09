/** \fuwu.cpp
 * 客户端，功能：UDP文字发送，TCP文件接收
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

#define RECEIVE_BUF_SIZE 1024
#define SEND_BUF_SIZE 100
#define SERVER_PORT 7801

int udp_send();
int tcp_recv();

int main()
{
	while(1)
	{
		printf("请选择需要的功能（1为文字发送，2为文件接收,0为退出）：\n");
		int func = 0;
		scanf("%d",&func);
		if(func == 1)
		{
			udp_send();
		}
		else if(func == 2)
		{
			printf("等待文件接收\n");
			tcp_recv();
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

/**\udp_send
 * @bref 使用UDP方法向服务端发送文字
 * 参数列表 无
 * 返回值 无意义
*/
int udp_send()
{
	//定义socket
	int sock_fd = 0;
	//定义发送数据的内存
	char sendbuf[SEND_BUF_SIZE] = {0};
	//定义接收数据中长度
	int sendlen = 0;
	
	//定义服务器的socket通讯地址
	struct sockaddr_in ser_addr;

	//设置服务器的socket参数
	memset(&ser_addr,0,sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	inet_aton("127.0.0.1",(struct in_addr*)&ser_addr.sin_addr);
	ser_addr.sin_port = htons(SERVER_PORT);
	
	while(1)
	{
		//创建socket
		sock_fd = socket(AF_INET,SOCK_DGRAM,0);
		if(sock_fd < 0)
		{
			printf("create socket failed\n");
			exit(1);
		}
	
		//发送数据
		//sprintf(sendbuf,"hello serber");
		printf("请输入你要发送的内容(输入q退出)：\n");
		fgets(sendbuf,SEND_BUF_SIZE,stdin); 
		if(!strcmp(sendbuf,"q\n"))
		{
			sendlen = strlen(sendbuf) + 1;
			int iRealSend = sendto(sock_fd,sendbuf,sendlen,0,
					(struct sockaddr*)(&ser_addr),sizeof(struct sockaddr_in));
			break;
		}
		sendlen = strlen(sendbuf) + 1;
		int iRealSend = sendto(sock_fd,sendbuf,sendlen,0,
					(struct sockaddr*)(&ser_addr),sizeof(struct sockaddr_in));
		printf("send data is:%s,send length:%d\n",sendbuf,iRealSend);
	}	
	//关闭socket
	close(sock_fd);
}

/**\tcp_recv
 * @bref 使用TCP方法接收服务端使用TCP发送的文件
 * 参数列表 无
 * 返回值 无意义
*/
int tcp_recv()
{
	//定义socket
	int sock_fd = 0;
	//定义接收文件名称的内存
	char file[RECEIVE_BUF_SIZE] = {0};
	//定义接收数据的内存
	char buf[RECEIVE_BUF_SIZE] = {0};
	
	//定义服务器的socket通讯地址
	struct sockaddr_in ser_addr;

	//设置服务器的socket参数
	memset(&ser_addr,0,sizeof(ser_addr));
	ser_addr.sin_family= AF_INET;
	inet_aton("127.0.0.1",(struct in_addr*)&ser_addr.sin_addr);
	ser_addr.sin_port = htons(SERVER_PORT);
	
	//创建socket
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	if(sock_fd < 0)
	{
		printf("create socket failed\n");
		exit(1);
	}

	//连接到服务器
	if(connect(sock_fd,(struct sockaddr*)&ser_addr,sizeof(ser_addr)) < 0)
	{
		printf("connect socket failed\n");
		close(sock_fd);
		exit(1);
	}

	//接收文件名称
	int receivelength = 0;
	while(receivelength <= 0)
	{
		receivelength = read(sock_fd,file,sizeof(file));
	}
	if(!strcmp(file,"q"))
	{
		printf("服务端已退出，程序结束\n");
		close(sock_fd);
		exit(0);
	}
	printf("receive name length:%d\n",receivelength);
	sprintf(buf,"%s_%s","receive",file);
	printf("begin receive file %s\n",file);

	//创建接收文件
	FILE *target;
	target = fopen(buf,"wb");
	if(target == NULL)
	{
		printf("open file failed\n");
		close(sock_fd);
		exit(1);
	}

	//通知服务器已经准备好了
	sprintf(buf,"ready");
	write(sock_fd,buf,strlen(buf));
	
	//开始接收文件内容
	memset(buf,0,RECEIVE_BUF_SIZE);
	while((receivelength = read(sock_fd,buf,RECEIVE_BUF_SIZE)) > 0)
	{
		if(receivelength < 0)
		{
			printf("receive file error\n");
			fclose(target);
			close(sock_fd);
			exit(1);
		}
		
		printf("receive file length:%d\n",receivelength);
		fwrite(buf,1,receivelength,target);
		memset(buf,0,RECEIVE_BUF_SIZE);
	}
	printf("receive file success\n");

	//关闭文件句柄
	fclose(target);
	
	//关闭socket
	close(sock_fd);
}
