#include<stdio.h>
#include<string.h>
int main(){
    char a[100];
    scanf("%[^\n]",&a);
    //scanf("%[^\n]", str);
    //gets(a);
    int i;
    int b=strlen(a);
    for(i=b-1;i>=0;i--){
        printf("%c",a[i]);
    }
    //printf("%s",c);
    return 0;
}
