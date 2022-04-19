#include <stdio.h>

int tiao7(int jk)
{
    if(jk % 7 == 0)
    {
        return 1;
    }
    while(jk)
    {
        if(jk % 10 == 7)
        {
            return 1;
        }
        jk = jk/10;
    }
    return 0;
}


int main()
{
    int n;
    scanf("%d",&n);
    int num = 0;
    for(int i = 1;i <= n;i++)
    {
        num += tiao7(i);
    }
    
    printf("%d",num);
    return 0;
}
