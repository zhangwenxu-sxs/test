#include <stdio.h>
#include <string.h>

int main()
{
    char arr[1000];
    gets(arr);
    int num_1 = 0,num_2 = 0,num_3 = 0,num_4 = 0;
    int len=strlen(arr);
    for(int i = 0;i < len;i++)
    {
        if(arr[i] >= 'a' && arr[i] <= 'z')
        {
            num_1++;
        }
        else if(arr[i] == ' ')
        {
            num_2++;
        }
        else if(arr[i] >= '0' && arr[i] <= '9')
        {
            num_3++;
        }
        else
        {
            num_4++;
        }
    }
    printf("%d\n%d\n%d\n%d",num_1,num_2,num_3,num_4);
    
    return 0;
}
