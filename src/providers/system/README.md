# System Scheme

The system scheme is a scheme to allow for different devices that are handled in similar matters to be bound under a 
single provider.

For example all serial communications will be handled as:
* `system://serial:<id>/`

with the id choosing which underlying device to use.

specifying an id of 0 will open the device in a mode which allows to read the configuration of the domain:

```c
struct system_domain {
    // entry count (port 1-...)
    size_t count;
} __attribute__((packed));
```

## How does it works
The way this works is that a provider who has supporting ABI will simply register the resource descirptor used to open 
the resource, and will forward any request to it.

# Supported Domains

## Keyboard

The keyboard domain is used to uniform any keyboard connected.

The ABI is pretty simple, each key has a virtual code which is one byte long. So each key read is one byte long.

## Serial

Serial communication is very similar to keyboard, but it is both an input and output device.

It does not have a special ABI and read/write should be written directly.

## Framebuffer

A framebuffer is anything which can output an image. Usually you will have one framebuffer per screen.

The ABI is that the size get be obtained using two special invokes:
* `FRAMEBUFFER_SIZE = 0xFFFFFFFF00000000`

which takes a struct in the form
```c
struct framebuffer_size {
    uint16_t width;
    uint16_t height;
} __attribute__((packed));
```

Also the format of the image is RGBA, the provider should transform it accordingly to how it needs
