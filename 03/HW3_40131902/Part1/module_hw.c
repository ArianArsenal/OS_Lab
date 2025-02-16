#include <linux/init.h>       // For module_init and module_exit macros
#include <linux/module.h>     // For module-related macros
#include <linux/kernel.h>     // For printk
#include <linux/moduleparam.h> // For module parameters

// Define arrays and variables to hold the parameters
static char *stringArray[3];  // An array of strings with a maximum of 3 elements
static long longArray[3];     // An array of longs
static int intArray[3];       // An array of integers
static short shortArray[3];   // An array of shorts

// Define the number of elements in the arrays
static int stringCount = 0;
static int longCount = 0;
static int intCount = 0;
static int shortCount = 0;

// Declare module parameters and descriptions
module_param_array(stringArray, charp, &stringCount, 0660);
MODULE_PARM_DESC(stringArray, "An array of strings");

module_param_array(longArray, long, &longCount, 0660);
MODULE_PARM_DESC(longArray, "An array of longs");

module_param_array(intArray, int, &intCount, 0660);
MODULE_PARM_DESC(intArray, "An array of integers");

module_param_array(shortArray, short, &shortCount, 0660);
MODULE_PARM_DESC(shortArray, "An array of shorts");

// Function to execute when the module is loaded
static int __init simple_module_init(void)
{
    int i;

    // Print the passed string array
    pr_info("Module loaded with the following string array parameters:\n");
    for (i = 0; i < stringCount; i++) {
        pr_info("stringArray[%d] = %s\n", i, stringArray[i]);
    }

    // Print the passed long array
    pr_info("Module loaded with the following long array parameters:\n");
    for (i = 0; i < longCount; i++) {
        pr_info("longArray[%d] = %ld\n", i, longArray[i]);
    }

    // Print the passed int array
    pr_info("Module loaded with the following int array parameters:\n");
    for (i = 0; i < intCount; i++) {
        pr_info("intArray[%d] = %d\n", i, intArray[i]);
    }

    // Print the passed short array
    pr_info("Module loaded with the following short array parameters:\n");
    for (i = 0; i < shortCount; i++) {
        pr_info("shortArray[%d] = %hd\n", i, shortArray[i]);
    }

    return 0;
}

// Function to execute when the module is removed
static void __exit simple_module_exit(void)
{
    pr_info("Removing module\n");
}

// Register the init and exit functions
module_init(simple_module_init);
module_exit(simple_module_exit);

// Module metadata
MODULE_LICENSE("GPL"); //GPL = GNU General Public License
MODULE_DESCRIPTION("simple module");
MODULE_AUTHOR("OS-Lab-Group");