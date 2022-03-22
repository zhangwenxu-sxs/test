#include <stdio.h>
#include <string.h>

int main()
{
    char arr[1000];

    printf("输入要反转的字符串：\n");
    scanf("%s",arr);
    
    int num = 0;
    
    while(1)
    {
        if(arr[num] == '\0')
        {
            break;
        }
        num++;
    }
    
    printf("反转结果：\n");
    for(int i = num -1;i >= 0;i--)
    {
        printf("%c",arr[i]);
    }
    printf("\n");
    return 0;
}
