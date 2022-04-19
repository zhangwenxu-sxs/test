#include <stdio.h>

int main(void)
{
    char num[8];
    //scanf("%s",num);
    gets(num);
    char result[8];
    int flag = 0;
    int n = 0;
    for(int i = strlen(num) -1;i >=0;i--)
    {
        flag = 0;
        for(int j = 0;j < n;j++)
        {
            if(result[j] == num[i])
            {
                flag = 1;
                break;
            }
        }
        if(flag == 0)
        {
            result[n++] = num[i];
        }
    }

    result[n]= '\000';
    printf("%s",result);
    
    
}
