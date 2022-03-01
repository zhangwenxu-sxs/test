#include <stdio.h>

int main()
{
    int n,tmp,sum = 0;
    
    printf("输入要相加的数字的数量：\n");
    while(scanf("%d",&n) != EOF)
    {
        printf("输入要相加的数字：\n");
        sum = 0;
        while(n--)
        {
            scanf("%d",&tmp);
            sum += tmp;
        }
        printf("%d\n",sum);
        printf("输入要相加的数字的数量：\n");
    }
    
    return 0;
}
