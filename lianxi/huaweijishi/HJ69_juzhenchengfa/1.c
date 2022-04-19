#include <stdio.h>

int main()
{
    int x,y,z;
    scanf("%d",&x);
    scanf("%d",&y);
    scanf("%d",&z);
    int a[x][y];
    int b[y][z];
    int result[x][z];

    for(int i = 0;i < x;i++)
    {
        for(int j = 0;j < y;j++)
        {
            scanf("%d",&a[i][j]);
        }
    }
    for(int i = 0;i < y;i++)
    {
        for(int j = 0;j < z;j++)
        {
            scanf("%d",&b[i][j]);
        }
    }
    
    for(int i = 0;i < x;i++)
    {
        for(int j = 0;j < z;j++)
        {
          result[i][j] = 0;
        }
    }
    
    
    for(int i = 0;i < x;i++)
    {
        for(int j = 0;j < z;j++)
        {
            for(int k = 0;k < y;k++)
            {
                result[i][j] = result[i][j] + a[i][k] * b[k][j];
            }
        }
    }
    
    for(int i = 0;i < x;i++)
    {
        for(int j = 0;j < z;j++)
        {
            printf("%d ",result[i][j]);
        }
        printf("\n");
    }
    
    
    
    return 0;
}
