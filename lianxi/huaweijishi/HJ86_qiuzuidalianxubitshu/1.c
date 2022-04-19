#include <stdio.h>


int main()
{
    int num;
    scanf("%d",&num);
    int max = 1;
    int n = 0;
    while(1)
    {
        if(num % 2 == 1)
        {
            n++;
        }
        else
        {
            n = 0;
        }
        if(n > max)
        {
            max = n;
        }
        if(num / 2 == 0)
        {
            break;
        }
        num = num / 2;
    }
    printf("%d\n",max);
    
    return 0;
}
