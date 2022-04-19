#include <stdio.h>

int main()
{
    int n;
    scanf("%d",&n);
    int num = 0;
    if(n == 1)
    {
        printf("1\n");
        return 0;
    }
    for(int i = 1;i <= n;i++)
    {
        int sum = 0;
        for(int j = 1;j < i;j++)
        {
            if(i%j == 0)
            {
                sum = sum + j;
            }
        }
        if(sum == i)
        {
            num++;
        }
    }
    printf("%d\n",num);
    
    return 0;
}
