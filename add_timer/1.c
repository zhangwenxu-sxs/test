#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#define TIME_WHEEL_SIZE 8

typedef void (*func)(int data);

struct timer_node {
    struct timer_node *next;
    int rotation;
    func proc;
    int data;
};

struct timer_wheel {
    struct timer_node *slot[TIME_WHEEL_SIZE];
    int current;
};

struct timer_wheel timer = {{0}, 0};

void tick(int signo)
{
    // 使用二级指针删进行单链表的删除
    struct timer_node **cur = &timer.slot[timer.current];
    while (*cur) {
        struct timer_node *curr = *cur;
        if (curr->rotation > 0) {
            curr->rotation--;
            cur = &curr->next;
        } else {
            *cur = curr->next;
            curr->proc(curr->data);  // bug-fix: 与下面一样交换位置
            free(curr);
        }
    }
    timer.current = (timer.current + 1) % TIME_WHEEL_SIZE;
    alarm(1);
}

void add_timer(int len, func action)
{
    int pos = (len + timer.current) % TIME_WHEEL_SIZE;
    struct timer_node *node = malloc(sizeof(struct timer_node));

    // 插入到对应格子的链表头部即可, O(1)复杂度
    node->next = timer.slot[pos];
    timer.slot[pos] = node;
    node->rotation = len / TIME_WHEEL_SIZE;
    node->data = 0;
    node->proc = action;
}

 // test case1: 1s循环定时器
int g_sec = 0;
void do_time1(int data)
{
    printf("timer %s, %d\n", __FUNCTION__, g_sec++);
    add_timer(1, do_time1);
}

// test case2: 2s单次定时器
void do_time2(int data)
{
    printf("timer %s\n", __FUNCTION__);
}

// test case3: 9s循环定时器
void do_time9(int data)
{
    printf("timer %s\n", __FUNCTION__);
    add_timer(9, do_time9);
}

int main()
{
    signal(SIGVTALRM, tick);
    alarm(1); // 1s的周期心跳

    // test
    add_timer(1, do_time1);
    add_timer(2, do_time2);
    add_timer(9, do_time9);

    while(1) pause();
    return 0;
}

