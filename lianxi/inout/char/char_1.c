#include <stdio.h>
#include <string.h>

int main()
{
    int num;
    printf("输入要排序的字符串数量：\n");
    scanf("%d",&num);
    
    char arr[num][100];
    
    printf("输入要排序的字符串：\n");
    for(int n = 0; n < num; n++)
    {
        scanf("%s",arr[n]);
    }
    
    qsort(arr,num,sizeof(arr[0]),strcmp);
    
    printf("排序结果：\n");
    for(int n = 0; n < num; n++)
    {
        printf("%s ",arr[n]);
    }
    printf("\n");   
    return 0;
}
