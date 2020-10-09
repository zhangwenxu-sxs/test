#include <stdio.h>





void a(int *aa)
{
    if(aa == NULL)
    {
        printf("aa = null");
    }
    else
    {
        printf("aa = %d", *aa);
    }
}



int main()
{
    int n = 10;
    int *l = &n;
    int *m = NULL;
    a(l);
    a();
    return 0;
}
