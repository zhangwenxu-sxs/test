#include<stdio.h>
#include<string.h>

int main()
{
    char ar[100][100];
    int num=0;
    while(scanf("%[^,\n]", ar[num++])!=EOF)
    {
        if(getchar() == '\n')
        {
            qsort(ar, num, sizeof(ar[0]), strcmp);
            for(int i=0; i<num-1; i++)
            {
                printf("%s,", ar[i]);
            }
            printf("%s\n", ar[num-1]);
            num = 0;
        }
    }
    return 0;
}
