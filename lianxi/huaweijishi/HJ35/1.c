#include <stdio.h>
int main()
{
    int n;
    scanf("%d",&n);
    int a1 = 1,
    tmp1 = 1,
    b1 = 1,
    b2 = 2,
    b22,
    tmp2;
    for(int i = n;i >=1 ;i--)
    {
        b22 = b2;
        for(int j = 0;j < i;j++)
        {
            printf("%d ",a1);
            a1 += b22;
            b22++;
        }
        a1 = tmp1 + b1;
        b1++;
        b2++;
        tmp1 = a1;
        printf("\n");
    }
    return 0;
}
