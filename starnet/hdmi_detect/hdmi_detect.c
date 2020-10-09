#include <linux/printk.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/switch.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>


static struct proc_dir_entry *hisi_hdmi_out_proc_root = NULL;  //hdmi dir
static struct proc_dir_entry *hisi_hdmi_out_st = NULL;         //hdmi state

static int __hisi_hdmi_out_st = 0;
static struct switch_dev hisi_hdmi_out = {
    .name = "hdmi"
};

static ssize_t __seq_write_hisi_hdmi_out_st(struct file *file,
		const char __user *buf, size_t count, loff_t *ppos)
{
    int val=0;

    printk("%s.%d buf=%s count=%zu \n",__FUNCTION__,__LINE__,buf,count);

    if(*buf == '0') {
        val = 0;
    } else {
        val = 1;
    }

    __hisi_hdmi_out_st = val;
    switch_set_state(&hisi_hdmi_out, val);

    return count;
}

static int __info_proc_hisi_hdmi_out_st(struct seq_file *m, void *v)
{
    seq_printf(m, "hisi_hdmi_state=%d\n",__hisi_hdmi_out_st);
    return 0;
}

static int __seq_open_hisi_hdmi_out_st(struct inode *inode, struct file *file)
{
    return single_open(file, &__info_proc_hisi_hdmi_out_st, NULL);
};

static const struct  file_operations __proc_fops_hisi_hdmi_out_st = {
    .owner  = THIS_MODULE,
    .open = __seq_open_hisi_hdmi_out_st,
    .read = seq_read,
    .write = __seq_write_hisi_hdmi_out_st,
    .llseek = seq_lseek,
    .release = seq_release,
};

static int  hisi_hdmi_out_proc_init(void)
{
    hisi_hdmi_out_proc_root = proc_mkdir("hisi_hdmi", NULL);
	if (hisi_hdmi_out_proc_root == NULL)
		return -ENOMEM;

    hisi_hdmi_out_st = proc_create("state", 0666, hisi_hdmi_out_proc_root, &__proc_fops_hisi_hdmi_out_st);
    if (hisi_hdmi_out_st == NULL) {
        printk("Unable to create %s/%s .\n", "hisi_hdmi_out","state");
        return -ENOMEM;
    }

    return 0;
}

static int hisi_hdmi_out_proc_uninit(void)
{
    if (hisi_hdmi_out_st != NULL) {
        proc_remove(hisi_hdmi_out_st);
    }

    if (hisi_hdmi_out_proc_root != NULL) {
        remove_proc_entry("hisi_hdmi", NULL);
    }

    return 0;
}

static int hdmi_detect_init(void)
{
    switch_dev_register(&hisi_hdmi_out);
    switch_set_state(&hisi_hdmi_out, 0);

    hisi_hdmi_out_proc_init();
    return 0;
}

static void hdmi_detect_exit(void)
{
    hisi_hdmi_out_proc_uninit();
    switch_dev_unregister(&hisi_hdmi_out);
}

module_init(hdmi_detect_init);
module_exit(hdmi_detect_exit);

MODULE_AUTHOR("ZHAOMENGEN");
MODULE_DESCRIPTION("HDMI DETECT CONVERT");
MODULE_LICENSE("GPL");
