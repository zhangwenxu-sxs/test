#include <stdio.h>

// #define min_t(x,y) ((x)<(y)?(x):(y))  
//#define min_t(type,x,y) \
         ({type __x=(x); type __y=(y);__x<__y?__x:__y;})
#define min_t(x,y)\
    ({typeof(x) __x=(x); typeof(y) __y=(y);__x<__y?__x:__y;})  
 

int main()
{
    int a = 0, b = 2;
    printf("a = %d, b = %d \n",a ,b);
    min_t(a++,++b);
    printf("a = %d, b = %d \n",a ,b);
}
