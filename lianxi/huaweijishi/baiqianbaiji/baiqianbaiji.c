#include <stdio.h>


int main()
{
    int p;
    scanf("%d",&p);
    
    for(int x = 0;x < 24;x++)
    {
        for(int y = 0;y < 33;y++)
        {
            for(int z = 0;z < 100;z++)
            {
                if(((5*x + 3*y + z) == 100) && 
                   ((x + y + 3*z) == 100))
                {
                    printf("%d %d %d\n",x,y,3*z);
                }
            }
        }
    }
    
    return 0;
}
