#include <stdio.h>

int main()
{
    int n;
    int sum = 0;
    int tmp;
    printf("输入要相加数的个数（输入0结束程序）：\n");
    while(scanf("%d",&n) != EOF)
    {
        if(n == 0)
        {
            break;
        }
        sum = 0;
        printf("输入要相加的数字：\n");
        for(n;n--;n>0)
        {
            scanf("%d",&tmp);
            sum += tmp;
        }
        printf("%d\n",sum);
        printf("输入要相加数的个数（输入0结束程序）：\n");
    }
    
    return 0;
}
