# PS2 provider

This is a super simple PS2 keyboard & mouse provider, the idea is to just have a simple way of having a mouse without implementing USB.

This provider supports the `poll` and `wait` syscalls.

## Keyboard

To open a handle to the keyboard use `ps2://keyboard/`

Then you can simply read each byte (which represents the scan code)

## Mouse

**TODO**