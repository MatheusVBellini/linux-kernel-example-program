#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <asm/io.h>

// Video memory address and size (adjust according to your system)
#define VIDEO_MEMORY_ADDRESS 0xde000000
#define VIDEO_MEMORY_SIZE    (1024 * 1024) // 1 MB

static void* video_mem_snapshot;

int init_module(void)
{
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

    // Write the snapshot into the kernel log
    printk(KERN_INFO "Video Memory Snapshot:\n");
    const unsigned char* snapshot = (const unsigned char*)snapshot_buffer;
    unsigned int i;
    for (i = 0; i < VIDEO_MEMORY_SIZE; ++i) {
        printk("%02X ", snapshot[i]);
        if ((i + 1) % 16 == 0)
            printk("\n");
    }

    vfree(snapshot_buffer);
    iounmap(video_mem_snapshot);

    return 0;
}

void cleanup_module(void)
{
    // Cleanup if necessary
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Angelo");
MODULE_DESCRIPTION("Video Memory Logger");