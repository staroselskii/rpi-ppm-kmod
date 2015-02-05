#include <linux/module.h>
#include <linux/kernel.h>

static int __init ppm_init(void)
{
    printk(KERN_INFO "PPM init\n");

    return 0;
}
static void __exit ppm_exit(void)
{
    printk(KERN_INFO "PPM exit\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Georgii Staroselskii");
MODULE_DESCRIPTION("Linux Kernel module for PPM handling.");

module_init(ppm_init);
module_exit(ppm_exit);
