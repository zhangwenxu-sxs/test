#include <stdio.h>
#include <string.h>

int main()
{
    char arr[500];
    
    scanf("%s",arr);
    
    int asc[128] = {0};
    for(int i = 0;i < strlen(arr);i++)
    {
        asc[arr[i]] = 1;
    }
    int num = 0;
    for(int i = 0;i < 127;i++)
    {
        num += asc[i];
    }
    
    printf("%d\n",num);
}
