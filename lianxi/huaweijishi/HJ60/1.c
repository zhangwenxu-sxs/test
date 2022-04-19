#include <stdio.h>


int main()
{
    int num;
    scanf("%d",&num);
    int flag1 = 0,flag2 = 0;
    int su1,su2;
    for(int i = num/2;i > 0;i--)
    {
        flag1 = 0;
        flag2 = 0;
        su1 = 0;
        su2 = 0;
        for(int j = 2;j < i;j++)
        {
            if(i % j == 0)
            {
                flag1 = 1;
                break;
            }
        }
        if(flag1 == 1)
        {
            continue;
        }
        
        
        for(int j = 2;j < num - i;j++)
        {
            if((num - i) % j == 0)
            {
                flag2 = 1;
                break;
            }
        }
        if(flag2 == 1)
        {
            continue;
        }
        printf("%d\n%d",i,num-i);
        break;
    }
    
    
    return 0;
}
