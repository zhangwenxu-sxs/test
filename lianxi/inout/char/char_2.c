#include <stdio.h>
#include <string.h>

int main()
{
    char arr[100][100];
    int n = 0;

    printf("输入要排序的字符串：\n");
    while(scanf("%s",arr[n]) != EOF)
    {
        n++;
        if(getchar() == '\n')
        {
            printf("排序结果：\n");
            qsort(arr,n,sizeof(arr[0]),strcmp);
            for(int m = 0; m < n; m++)
            {
                printf("%s ",arr[m]);
            }
            n = 0;
            printf("\n");
            printf("输入要排序的字符串：\n");
        }
        
    }
    
    
    return 0;
}
