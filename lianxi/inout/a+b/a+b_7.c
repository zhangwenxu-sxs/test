#include <stdio.h>

int main()
{
    int tmp,sum = 0;
    printf("输入要相加的数字：\n");
    while(scanf("%d",&tmp) != EOF)
    {
        sum += tmp;
        if(getchar() == '\n')
        {
            printf("%d\n",sum);
            printf("输入要相加的数字：\n");
            sum = 0;
        }
    }
    return 0;
}
