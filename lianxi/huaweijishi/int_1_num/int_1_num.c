#include <stdio.h>

int main()
{
    int num;
    scanf("%d",&num);
    int num_1 = 0;
    while(num)
    {
        if(num % 2 == 1)
        {
            num_1++;
        }
        num = num /2;
    }
    printf("%d\n",num_1);
    return 0;
}
