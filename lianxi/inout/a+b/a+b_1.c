#include <stdio.h>


int main()
{
    int a,b;
    printf("输入相加的两个数：\n");
    while(scanf("%d %d",&a,&b) != EOF)
    {
        printf("%d \n",a+b);
        printf("输入相加的两个数：\n");
    }
    return 0;
}
