#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>

/* Define the structure for birthday */
struct birthday {
    int day;
    int month;
    int year;
    struct list_head list; // Kernel's list structure
};

/* Initialize the head of the linked list */
static LIST_HEAD(birthday_list);

/* This function is called when the module is loaded */
int simple_init(void) {
    struct birthday *person;
    int i;

    printk(KERN_INFO "Loading Module\n");

    /* Create 5 birthday instances and add them to the linked list */
    for (i = 1; i <= 5; i++) {
        person = kmalloc(sizeof(*person), GFP_KERNEL); // Allocate memory
        if (!person) {
            printk(KERN_ALERT "Memory allocation failed\n");
            return -ENOMEM;
        }
        person->day = i;
        person->month = i + 1;
        person->year = 1990 + i;
        INIT_LIST_HEAD(&person->list);  // Initialize list structure
        list_add_tail(&person->list, &birthday_list);  // Add to list

        printk(KERN_INFO "Added person: %d/%d/%d\n", person->day, person->month, person->year);
    }

    /* Traverse and print the list */
    printk(KERN_INFO "Traversing the birthday list\n");
    list_for_each_entry(person, &birthday_list, list) {
        printk(KERN_INFO "Birthday: %d/%d/%d\n", person->day, person->month, person->year);
    }

    return 0;
}

/* This function is called when the module is removed */
void simple_exit(void) {
    struct birthday *person, *next;

    printk(KERN_INFO "Removing Module\n");

    /* Reverse traverse the list and print */
    printk(KERN_INFO "Reversing the birthday list\n");
    list_for_each_entry_reverse(person, &birthday_list, list) {
        printk(KERN_INFO "Birthday: %d/%d/%d\n", person->day, person->month, person->year);
    }

    /* Delete the list and free memory */
    list_for_each_entry_safe(person, next, &birthday_list, list) {
        printk(KERN_INFO "Deleting person: %d/%d/%d\n", person->day, person->month, person->year);
        list_del(&person->list);  // Remove from list
        kfree(person);  // Free memory
    }
}

/* Registering the init and exit functions */
module_init(simple_init);
module_exit(simple_exit);

/* License and description information */
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple birthday linked list module");
MODULE_AUTHOR("OS-Lab-Group");
