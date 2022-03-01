#include <stdio.h>

int main()
{
    int n;
    int a,b;
    printf("输入要相加的数组数量：\n");
    scanf("%d",&n);
    for(n;n--;n>0)
    {
        printf("输入要相加的两个数：\n");
        scanf("%d %d",&a,&b);
        printf("%d\n",a+b);
    }   
    return 0;
}
