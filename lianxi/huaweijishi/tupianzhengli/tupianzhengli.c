#include <stdio.h>
#include <string.h>

int main()
{
    char arr[1000];
    scanf("%s",arr);
    
    qsort(arr,strlen(arr),sizeof(char),strcmp);
    
    printf("%s\n",arr);
    
    
    return 0;
}
