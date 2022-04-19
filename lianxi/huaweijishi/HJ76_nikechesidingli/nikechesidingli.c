#include <stdio.h>


int main()
{
    int num;
    scanf("%d",&num);
    int sum[num];
    int num_3 = num * num * num;
    int ave = num_3 / num;
    if(ave % 2 == 1)
    {
        for(int i = 0;i < num;i++)
        {
            sum[i] = ave + 2 * (i - (num / 2));
        }
    }
    else
    {
        for(int i = 0;i < num;i++)
        {
            sum[i] = ave + 2 * (i - (num / 2)) + 1;
        }
    }
    for(int i = 0;i < num;i++)
    {
        printf("%d",sum[i]);
        if(i == num -1)
        {
            break;
        }
        printf("+");
    }
    printf("\n");
    return 0;
}
