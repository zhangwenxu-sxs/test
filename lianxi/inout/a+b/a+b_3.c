#include <stdio.h>

int main()
{
    int a,b;
    printf("输入需要相加的两个数(输入：0 0 结束程序)：\n");
    while (scanf("%d %d",&a,&b) != EOF)
    {
        if(a == 0 && b == 0)
        {
            break;
        }
        printf("%d\n",a+b);
        printf("输入需要相加的两个数(输入：0 0 结束程序)：\n");
    }
    
    
    return 0;
}
