#include<stdio.h>
int main(){
    int input;
    #define total 1000
    int a[total][2*(total-1)+1]={{1},{1,1,1,}};
    while(scanf("%d", &input) != EOF){
    if(input <=2) {
        printf("-1\n");
    }
    for(int m = 2; m < input; m++) {
        a[m][0] = 1;
        for(int n = 1; n < (2*(m-1) + 1); n++) {
            if(n == 1){
                a[m][n] = a[m-1][0] + a[m-1][1];
            } else {
                a[m][n] = a[m-1][n-2] + a[m-1][n-1] + a[m-1][n];
            }
            if(m == (input -1) && a[m][n]%2==0){
                printf("%d\n",n+1);
                break;
            }
        }
    }
    }
}
