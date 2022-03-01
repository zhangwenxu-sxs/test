#include <stdio.h>

int main()
{
    int n,m;
    int sum = 0;
    int tmp;
    printf("输入要相加的数组数量：\n");
    scanf("%d",&n);
    while(n--)
    {
        sum = 0;
        printf("输入想相加的数的数量：\n");
        scanf("%d",&m);
        printf("输入要相加的数：\n");
        while(m--)
        {
            scanf("%d",&tmp);
            sum += tmp;
        }
        printf("%d\n",sum);
    }
    
    return 0;
}
