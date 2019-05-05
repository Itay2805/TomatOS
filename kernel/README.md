# TomatKernel

## Note: This is mostly conceptual, and not implemented yet

TomatKernel is going to be a microkernel. Everything is going to be done by a set of simple syscalls:

* open
* close
* read
* write
* invoke
* poll
* wait
* seek
* tell

The way open works is by getting a `resource_descriptor`, this descriptor can basically be defined as a url, where we have a scheme, port, path and arguments. There is also nesting of resources, which some will be resolved automatically by the provider.

### Example

`tcp://127.0.0.1:8192/`

will be

```json
{
    "scheme": "tcp",
    "domain": "127.0.0.1",
    "port": 8192
}
```

but it will actually be resolved to:

`tcp://[ipv4://[net://ether:0/]127.0.0.1/]:8192/`

or

```json
{
    "scheme": "tcp",
    "port": 8192,
    "sub": {
        "scheme": "ipv4",
        "domain": "127.0.0.1",
        "sub": {
            "scheme": "net",
            "domain": "ether",
            "port": 0
        }
    }
}
```

basically the tcp provider will open an ipv4 resource, which it can read and write to, the ipv4 resource will open a net resource on the first ethernet port. of course if we really want we can do some fun stuff:

`tcp://[serial://:0/]:8192/` which will basically do

```json
{
    "scheme": "tcp",
    "port": 8192,
    "sub": {
        "scheme": "serial",
        "port": 0
    }
}
```

meaning it will use the serial port as the resource for writing the tcp on, it means it will read and write to the serial port the tcp protocol.

A none networking example is with file systems:

`file:///some/path/to/file`

file is a vfs, which we can mount stuff into, so lets assume we mounted `ext4://[ide://master:0/]/` to `file:///` then we will get:

```json
{
    "scheme": "ext4",
    "path": "/some/path/to/file",
    "ide": {
        "scheme": "ide",
        "domain": "master",
        "port": 0
    }
}
```

so this will take the file vfs, which will do the translation to actual fs, in this case ext4, which will basically read from the ide master controller in port 0.

we then can also do stuff like:

`ext4://[file:///some/image]/some/path`

which will basically mean we will read the file as a disk source, allowing us to actually mount it as a file system.


This system gives us an easy to use interface for programs to interact with hardware, we can also implement stuff like:

`zero:///` - act as /dev/null and /dev/zero

`http://test.com/` - we can make stuff like that, and using the invoke we can write such code:

```c
// some buffer to read into
char buffer[1024];

// resolve the url for the scheme and such and open 
// a resource
// Note: there is also resolve and open separate, so you
//       can resolve once and use the same resolved thing
resource_t http_res = resolve_and_open("http://example.com/");

// read the response
// this will send the request automatically, with get being 
// the default method
int read = read(http_res, buffer, 1024);

// print it
printf(read);

// never forget to close the resource
close(http_res);
```

any process can implement schemes, with some low level schemes (like `net`, `ide` and alike) being implemented as a kernel process but some (like `tcp`, `ext4`) and alike being implemented in user space. making this system easily extensible, and the user side library will also allow for process local providers, so it will proxy the syscall and search for the provider locally before making the syscall.

Another cool thing we can implement is a caching provider, which will track the access of the resource and will try to cache it if possible, can be easily used on file system mounts like:

`ext4://[cache://[ide://master:0/]/]/`

which will mount ext4 ontop of a cache, this cache will use the ide master controller port 0 as the input, but will cache depending whenever possible.

we can also have event systems like:

`system://memory/free_mem`

if we open this resource and wait on it, we will be triggered whenever there is low memory and that we should free some memory. we can then notify the system (`invoke(sys_mem_res, SYS_MEM_FREED, 0)`) so the system will know to try again now.

combined with the caching we can write a smart caching that will uncache when there is low memory.

we can also create timers:

```c
// will open an interval timer with 100 seconds interval
resource_t timer_res = resolve_and_open("timer://interval:100/");

// will set the timer to the given amount of millis
// so in here we change the interval to 600
invoke(timer_res, TIMER_SET_INTERVAL_MILLIS, 600);

while(true) {
    // wait for the 600 millis to pass
    wait(timer_res);

    // do something
}

// close it when finished
close(timer_res);
```

## Implementation

The way this is going to be implemented is that the syscall will basically delegate itself to the correct handler by copying the whole resolved descriptor to the kernel, creating a new thread that will handle the syscall in the provider's process, and suspending the caller (if blocking). eventually the provider will finish the thing, and will tell the kernel (will happen automatically with a small trampoline injected to the process that calls a syscall in the end), the kernel will then copy the result to the caller, kill the handling thread, and set the caller as ready to run.

This allows processes to have a global context, which is available to them at all times.


## Syscall path

A syscall starts when a requesting process calls one of the queueing syscalls (open, close, etc..)

We are now in kernel context, in here we set the thread to suspended, and set the resource it is waiting on to the given resource. after doing this we will create a new thread on the provider process, that thread is going to start at a trampoline. This trampoline is going to save the pid, the tid and call the actual handler (the reason we do this will be explained later). Afterwards we will simply call the scheduler to schedule a new thread to run (will most probably be the one we just created since it has the least amount of time).

Lets say we are now in the provider's thread context. as said it will push the pid, tid and call the actual handler with the correct parameters. once it returns we are going to call a special syscall that will tell the kernel we finished with the handling, it will be given the pid, tid and the resource we finished on.

now we are back in kernel space, this time we are going to find the thread that waited, pass the result to it, and set it to normal state, now it is ready to run again, now that the resource handling is over.

### The problem with this system
As much as I like this system because it allows to make any process into a provider technically (for starters we are only going to allow kernel processes to do that), it requires alot of context switches, and even if we don't do full ones if it is for drivers (which are in kernel space so the actual address space is not switched) it still means creating threads, running them and then resuming the process thread. it does mean we don't block too much in the kernel, we only do the dispatching.