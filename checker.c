/*
 * Tested on ubuntu 5.3.14 kernel
 *
 * Note: This module prints syscall virtual addresses and their current ponter values
 *
 * Compilation:
 * KERNEL_ROOT=/path/to/kernel/linux-5.3.14 make
 *
 * Usage:
 * sudo insmod checker.ko message='"some message"' #load module
 * sudo dmesg || sudo tail /var/log/kern.log #check the logs
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <asm/pgtable.h>
#include <linux/slab.h>
#include <linux/syscalls.h>

// Defining the module message parameter
static char *message = NULL;
module_param(message, charp, 0);
MODULE_PARM_DESC(message, "Message to log something in regards the check.");


/* pointer to store the syscall table address */
uint64_t *__sys_call_table;

/* Get the sys_call_table address using kallsys_lookup_name() */
uint64_t *get_syscall_table_bf(void)
{
	uint64_t *syscall_table;

	syscall_table = (uint64_t*)kallsyms_lookup_name("sys_call_table");
	return syscall_table;
}

/* ----------------------- INIT MODULE ----------------------- */

static int __init syshook_init(void) {

	printk(KERN_INFO "---------------------------------------------\n");
	printk(KERN_INFO "Init CHECKER module\n");
	printk("Message: %s\n", message);

	uint64_t *ptr;

	__sys_call_table = get_syscall_table_bf();

	ptr = (uint64_t*)__sys_call_table[0];

        printk("syscall_table VA: %llx\n", *__sys_call_table);

	printk("__x64_sys_kill: %llx\n", __sys_call_table[__NR_kill]);

	printk("__x64_sys_getdents: %llx\n", __sys_call_table[__NR_getdents]);

	return 0;
}

/* ----------------------- EXIT MODULE ----------------------- */

static void __exit syshook_cleanup(void) {
	printk(KERN_INFO "Cleaning up CHECKER module\n");
	printk(KERN_INFO "---------------------------------------------\n");
}

/* ------------------ CALLING INIT AND EXIT ------------------ */

module_init(syshook_init);
module_exit(syshook_cleanup);

/* ----------------------- MODULE INFO ----------------------- */

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("jpdejavitecs & cryptocrawll");
MODULE_DESCRIPTION("A module to check syscall addresses");
