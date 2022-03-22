#include <stdio.h>
#include <string.h>

int main()
{
    char arr[30];
    
    int n;
    printf("输入要反转的数字：\n");
    scanf("%d",&n);

    if(n == 0)
    {
        printf("0\n");
        return 0;
    }

    int num = 0;

    while(1)
    {
        if(n == 0)
        {
            break;
        }
        arr[num] = n % 10 + 48;
        n = n / 10;
        num++;
    }

    printf("反转结果：\n");
    for(int i = 0;i < num;i++)
    {
        printf("%c",arr[i]);
    }
    printf("\n");
    return 0;
}
