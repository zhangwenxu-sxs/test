#include<linux/init.h>
#include<linux/slab.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/list.h>

struct student 
{
    int id;
    char* name;
    struct list_head list;
};

void print_student(struct student*);
 
static int testlist_init(void) 
{
    struct student *stu1, *stu2, *stu3, *stu4;
    struct student *stu;
    // init a list head
    LIST_HEAD(stu_head);
    // init four list nodes
    stu1 = kmalloc(sizeof(*stu1), GFP_KERNEL);
    stu1->id = 1;
    stu1->name = "wyb";
    INIT_LIST_HEAD(&stu1->list);
    stu2 = kmalloc(sizeof(*stu2), GFP_KERNEL);
    stu2->id = 2;
    stu2->name = "wyb2";
    INIT_LIST_HEAD(&stu2->list);
    stu3 = kmalloc(sizeof(*stu3), GFP_KERNEL);
    stu3->id = 3;
    stu3->name = "wyb3";
    INIT_LIST_HEAD(&stu3->list);
    stu4 = kmalloc(sizeof(*stu4), GFP_KERNEL);
    stu4->id = 4;
    stu4->name = "wyb4";
    INIT_LIST_HEAD(&stu4->list);
    // add the four nodes to head
    list_add (&stu1->list, &stu_head);
    list_add (&stu2->list, &stu_head);
    list_add (&stu3->list, &stu_head);
    list_add (&stu4->list, &stu_head);
    // print each student from 4 to 1
    list_for_each_entry(stu, &stu_head, list)
    {
        print_student(stu);
    }
    // print each student from 1 to 4
    list_for_each_entry_reverse(stu, &stu_head, list)
    {
        print_student(stu);
    }
    // delete a entry stu2
    list_del(&stu2->list);
    list_for_each_entry(stu, &stu_head, list)
    {
        print_student(stu);
    }
    // replace stu3 with stu2
    list_replace(&stu3->list, &stu2->list);
    list_for_each_entry(stu, &stu_head, list)
    {
        print_student(stu);
    }
    return 0;
}

static void testlist_exit(void)
{
    printk(KERN_ALERT "*************************\n");
    printk(KERN_ALERT "testlist is exited!\n");
    printk(KERN_ALERT "*************************\n");
}

void print_student(struct student *stu)
{
    printk (KERN_ALERT "======================\n");
    printk (KERN_ALERT "id  =%d\n", stu->id);
    printk (KERN_ALERT "name=%s\n", stu->name);
    printk (KERN_ALERT "======================\n");
}

MODULE_LICENSE("GPL");

module_init(testlist_init);

module_exit(testlist_exit);
