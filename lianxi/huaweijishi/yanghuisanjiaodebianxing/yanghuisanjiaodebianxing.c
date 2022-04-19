#include <stdio.h>

int yanghui(int n,int m)
{
    if(m < 0 || m > n + 2 || n < 0)
    {
        return 0;
    }
    if(m == 0 || m == n+2 || n <= 1)
    {
        return 1;
    }

    return yanghui(n-1,m-1) 
        + yanghui(n-1,m) 
        + yanghui(n-1,m+1);
}


int main()
{
    int num;
    scanf("%d",&num);
    
    for(int i = 0;i < (2*num -1);i++)
    {
        if(yanghui(num -1,i) % 2 == 0)
        {
            printf("%d\n",i+1);
            return 0;
        }
    }
    printf("-1\n");
    
    return 0;
}
