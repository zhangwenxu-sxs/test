#include <stdio.h>


int main()
{
    double num;
    scanf("%lf",&num);
    
    int n = 0;
    if((num - (int)num) >= 0.5)
    {
        n = 1;
    }
    
    printf("%d\n",(int)num + n);
    
    return 0;
}
