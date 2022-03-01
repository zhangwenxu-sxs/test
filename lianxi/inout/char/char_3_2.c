#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main()
{
    char b;
    char a[100][100] = {0};
    int i = 0, j = 0, k = 0;
    while (scanf("%c", &b) != EOF)
    {
        if (b == ',')
        {
            i++;
            k = 0;
        }
        else if (b == '\n')
        {
            qsort(a, i + 1, sizeof(a[0]), strcmp);

            for (j = 0; j <= i; j++)
            {
                printf("%s", a[j]);
                if (j < i)
                {
                    printf(",");
                }
                else
                {
                    printf("\r\n");
                }
            }
            memset(a, 0, sizeof(a));
            i = 0;
            k = 0;
        }
        else
        {
            a[i][k++] = b;
        }
    }
}
