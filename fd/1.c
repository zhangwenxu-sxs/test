#include <stdio.h>
//chatter1.c
int main(int args,char *argv[])
{
    ARG_CHECK(args,3);
    int fdr,fdw;
    fdr = open(argv[1],O_RDONLY);//存放要接收的信息的文件
    fdw = open(argv[2],O_WRONLY);//存放要发送的信息的文件
    printf("小明,fdr=%d,fdw=%d\n",fdr,fdw);
    char sentence[1024] = {0};

    fd_set rdset;//文件描述符数组，数组中的文件描述符需要手动添加
    int fdReadyNum;//计数
    int ret;    //返回值
    struct timeval tc;//等待时间

    while(1)
    {
        FD_ZERO(&rdset);//清空描述符集合
        FD_SET(STDIN_FILENO,&rdset);//将标准输入流放入集合中
        FD_SET(fdr,&rdset);     //将收到的消息存放文件放入集合中
        tc.tv_sec =999;
        tc.tv_usec = 0;

        fdReadyNum = select(fdr+1,&rdset,NULL,NULL,&tc);

        //监控到就绪描述符
        if(fdReadyNum>0)
        {
            //标准输入就绪，则接收字符串写入fdw中
            if(FD_ISSET(STDIN_FILENO,&rdset))
            {
                memset(sentence,0,sizeof(sentence));

                ret = read(0,sentence,sizeof(sentence)-1);

                //标准输入没有输入内容
                if(0==ret)
                {
                    printf("下线\n");
                    break;
                }
                write(fdw,sentence,strlen(sentence)-1);
            }
            //fdr文件可读，代表管道内有新消息
            if(FD_ISSET(fdr,&rdset))
            {
                memset(sentence,0,sizeof(sentence));
                ret = read(fdr,sentence,sizeof(sentence));
                if(0==ret)
                {
                    printf("系统提示：对方已下线\n");
                    break;
                }
                printf("小红:\n%s\n小明：\n",sentence);
            }
        }
        else
        {
            printf("超时无应答，连接已断开！\n");
        }
    }
    return 0;
}


