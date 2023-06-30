# linux-kernel-example-program

This is an example program of kernel programming and kernel module development.

### Disclaimer
Kernel programs can be malicious and harmful to the machine, user discretion in its use is advised. The project is for educational purposes only and should not be used otherwise.

## Keyboard Module
The first module developed was the 'keyboard' module. Once loaded the module hosts a server in the kernel that makes available to any potential client the information of which keyboard keys are being pressed in the system.

The connection to the module is estabilished via ipv4 and the exchange of data is done using TCP.

### Functions in keyboard_test.c

```
static int keyboard_callback(struct notifier_block *nblock, unsigned long code, void *_param)
```
Responsible for acquiring the keycodes of the keys being pressed and sending as message to the client.

```
static int accept_thread_fn(void *data)
```
Function that runs on  a thread waiting for clients requesting connections to be accepted.

```
static int create_socket(void)
```
Creates the socket for client-server communication.

## Video Module
The second module developed was the 'video' module. Once loaded, as well as the keyboard module, it host a server but now the information that it sends to its client is a buffer containig information of what is currently being display on the system's screen.

The connection to the module is estabilished via ipv4 and the exchange of data is done using TCP.

### Functions in video_test.c
```
static int get_memory_thread_fn(void *data)
```
Function run in a thread to acquire pieces of video memory and send to the client.

```
static int accept_thread_fn(void *data)
```
```
static int create_socket(void)
```
Same functionality as the functions in ``keyboard_test.c``.

### How to run the modules

In order to load the modules, it is first necessary to compile the code into kernel object files (.ko), for that simply use the following command in the base folder of the program:

```
make
```

This will automatically generate `` keyboard_test.ko `` and `` video_test.ko ``. Now to load the modules into the kernel use the following commands:

```
sudo insmod keyboard_test.ko
```

```
sudo insmod video_test.ko
```

That's it! Now the modules are loaded into the kernel.

### Removing the modules
In order to remove the modules from the kernel, simply use the following commands:

```
sudo rmmod keyboard_test
```

```
sudo rmmod video_test
```

## Client
The client application differently from the other is not a kernel application, it is only a python program that can be used for acquiring the information that both the keyboard and the video modules are sending. After fetching the information, the client stores it in a file inside the ./log folder.

### How to run
The client can be used for two different purposes:

1. Connect to the keyboard module

2. Connect to the video memory module

In order to accomplish (1), use

```
make keyboard
```

In order to accomplish (2), use

```
make video
```

## Developers
Ângelo Antônio B. Guido, Bárbara F. Madera and Matheus V. Bellini
