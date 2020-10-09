#include <stdio.h>


void swap(int *a,int *b)
{
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
    printf("a = %d, b = %d \n",*a,*b);
}


void main()
{
    int a = 100, b = 200;
    int *p = &a;
    int *q = &b;
    printf("a = %d, b = %d \n",a,b);
    swap(p,q);
    printf("a = %d, b = %d \n",a,b);
}
