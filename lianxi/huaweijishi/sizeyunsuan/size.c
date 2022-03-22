#include <stdio.h>
#include <string.h>

// 1.无括号 用栈先乘除后加减
// 遇到+-继续入栈,遇到*/出栈1个,和新元素计算后入栈
// 2.有括号 左括号开始递归1步骤,右括号出递归结果 入栈
//注意遇到括号内有负号先要变号再入栈
int i;
int compute(char *data);
int main()
{
    
    char str[1000];
    scanf("%s", str);
    i=0;
    int res = compute(str);
    printf("%d", res);

    return 0;
}

int compute(char *data)
{
    int stack[1000];
    int top = -1;
    int len = strlen(data);
    char flag = '+';
    while (i <=len)
    {
        int num = 0;
        if(data[i]=='-'){
            flag = '-';
            i++;
        }
        
        if (data[i] == '{' || data[i] == '[' || data[i] == '(')
        {
            i++;
            num = compute(data);
        }

        while (data[i] - '0' >= 0 && data[i] - '0'<=9)
        {
            num = num * 10 + data[i] - '0';
            i++;
        }

        switch(flag)
        {
        case '+':
            stack[++top] = num;
            break;
        case '-':
            stack[++top] = -num;
            break;
        case '*':
            stack[top] *= num;
            break;
        case '/':
            stack[top] /= num;
            break;
        }
        flag = data[i];
        if (data[i] == '}' || data[i] == ']' || data[i] == ')')
        {
            i++;
            break;
        }
        i++;
    }

    int ret = 0;
    for (int j = 0; j <= top; j++)
    {
        ret += stack[j];
    }

    return ret;
}

