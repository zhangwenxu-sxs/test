#include <stdio.h>
#include "micro_test.h"
#include "micro_test.h"


int tab[10][10][10];

int func(int(* a)[10][10])
{
    a = tab;
    tab[1][1][2] = 10;
    printf("tab[1][1] = %d \n", tab[1][1][2]);
    
    printf("a[1][1] = %d \n", a[1][1][2] );
   

   

    return 0;
}

int main(int argc, char* argv[])
{
    int a;
    int* p = &a;
    int* pp = p + 1;

    func(tab);

    return 0;
}

