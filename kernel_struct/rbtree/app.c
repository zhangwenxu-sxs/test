/**
 * 根据Linux Kernel定义的红黑树(Red Black Tree)
 *
 * @author skywang
 * @date 2013/11/18
 */

#include <stdio.h>
#include <stdlib.h>
#include "rbtree.h"

#define CHECK_INSERT 0    // "插入"动作的检测开关(0，关闭；1，打开)
#define CHECK_DELETE 0    // "删除"动作的检测开关(0，关闭；1，打开)
#define LENGTH(a) ( (sizeof(a)) / (sizeof(a[0])) )

typedef int Type;

struct my_node {
    struct rb_node rb_node;    // 红黑树节点
    Type key;                // 键值
    // ... 用户自定义的数据
};

/*
 * 查找"红黑树"中键值为key的节点。没找到的话，返回NULL。
 */
struct my_node *my_search(struct rb_root *root, Type key)
{
    struct rb_node *rbnode = root->rb_node;

    while (rbnode!=NULL)
    {
        struct my_node *mynode = container_of(rbnode, struct my_node, rb_node);

        if (key < mynode->key)
            rbnode = rbnode->rb_left;
        else if (key > mynode->key)
            rbnode = rbnode->rb_right;
        else
            return mynode;
    }

    return NULL;
}

/*
 * 将key插入到红黑树中。插入成功，返回0；失败返回-1。
 */
int my_insert(struct rb_root *root, Type key)
{
    struct my_node *mynode; // 新建结点
    struct rb_node **tmp = &(root->rb_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp)
    {
        struct my_node *my = container_of(*tmp, struct my_node, rb_node);

        parent = *tmp;
        if (key < my->key)
            tmp = &((*tmp)->rb_left);
        else if (key > my->key)
            tmp = &((*tmp)->rb_right);
        else
            return -1;
    }

    // 如果新建结点失败，则返回。
    if ((mynode=malloc(sizeof(struct my_node))) == NULL)
        return -1;
    mynode->key = key;

    /* Add new node and rebalance tree. */
    rb_link_node(&mynode->rb_node, parent, tmp);
    rb_insert_color(&mynode->rb_node, root);

    return 0;
}

/*
 * 删除键值为key的结点
 */
void my_delete(struct rb_root *root, Type key)
{
    struct my_node *mynode;

    // 在红黑树中查找key对应的节点mynode
    if ((mynode = my_search(root, key)) == NULL)
        return ;

    // 从红黑树中删除节点mynode
    rb_erase(&mynode->rb_node, root);
    free(mynode);
}

/*
 * 打印"红黑树"
 */
static void print_rbtree(struct rb_node *tree, Type key, int direction)
{
    if(tree != NULL)
    {
        if(direction==0)    // tree是根节点
            printf("%2d(B) is root\n", key);
        else                // tree是分支节点
            printf("%2d(%s) is %2d's %6s child\n", key, rb_is_black(tree)?"B":"R", key, direction==1?"right" : "left");

        if (tree->rb_left)
            print_rbtree(tree->rb_left, rb_entry(tree->rb_left, struct my_node, rb_node)->key, -1);
        if (tree->rb_right)
            print_rbtree(tree->rb_right,rb_entry(tree->rb_right, struct my_node, rb_node)->key,  1);
    }
}

void my_print(struct rb_root *root)
{
    if (root!=NULL && root->rb_node!=NULL)
        print_rbtree(root->rb_node, rb_entry(root->rb_node, struct my_node, rb_node)->key,  0);
}


void main()
{
    int a[] = {10, 40, 30, 60, 90, 70, 20, 50, 80};
    int i, ilen=LENGTH(a);
    struct rb_root mytree = RB_ROOT;

    printf("== 原始数据: ");
    for(i=0; i<ilen; i++)
        printf("%d ", a[i]);
    printf("\n");

    for (i=0; i < ilen; i++)
    {
        my_insert(&mytree, a[i]);
//#if CHECK_INSERT
        printf("== 添加节点: %d\n", a[i]);
        printf("== 树的详细信息: \n");
        my_print(&mytree);
        printf("\n");
//#endif

    }

//#if CHECK_DELETE
    for (i=0; i<ilen; i++) {
        my_delete(&mytree, a[i]);

        printf("== 删除节点: %d\n", a[i]);
        printf("== 树的详细信息: \n");
        my_print(&mytree);
        printf("\n");
    }
//#endif
}
