#include <stdio.h>
//chatter2.c
int main(int args,char *argv[])
{
    ARG_CHECK(args,3);

    int fdr,fdw;
    fdw = open(argv[1],O_WRONLY);//存放发送信息的文件
    fdr = open(argv[2],O_RDONLY);//存放接收信息的文件
    printf("小红,fdr=%d,fdw=%d\n",fdr,fdw);     
    char sentence[1024] = {0};

    fd_set rdset;//文件描述符数组，数组中的文件描述符需要手动添加
    int fdReadyNum;//计数
    int ret;    //返回值
     printf("小红：\n");   
    while(1)
    {
        FD_ZERO(&rdset);//清空描述符集合
        FD_SET(STDIN_FILENO,&rdset);//将标准输入流放入集合中
        FD_SET(fdr,&rdset);     //将收到的消息存放文件放入集合中

        fdReadyNum = select(fdr+1,&rdset,NULL,NULL,NULL);
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
            printf("小明：\n%s\n小红:\n",sentence);
        }
    }
    return 0;
}



