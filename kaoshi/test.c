#include <stdio.h>
//#include "my.h"

enum {BUFFERSIZE = 512};

int  main(void)
{

    int a = strcmp("ab","abc");
    printf("%d\n",a);
    char buffer[BUFFERSIZE];
    return 0;
}
