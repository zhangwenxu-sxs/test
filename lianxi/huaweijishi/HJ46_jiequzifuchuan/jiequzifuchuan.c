#include <stdio.h>
#include <string.h>


int main()
{
    char arr[1000];
    int num;
    scanf("%s",arr);
    
    scanf("%d",&num);
    for(int i = 0;i < num;i++)
    {
        printf("%c",arr[i]);
    }
    printf("\n");
    
    return 0;
}
