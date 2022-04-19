#include<stdio.h>
#include<string.h>

int main()
{
	char str[250];
	 //while(scanf("%[^\n]",str)!=EOF)
     while(gets(str))
	 {
         int s = 0;
         for(int i = 0;i<strlen(str);i++)
         {
			if(str[i]>='A'&&str[i]<='Z')
			{
				s++;
			}
         }
	printf("%d\n",s);
   }
    return 0; 
}
