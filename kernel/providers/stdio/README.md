# stdio provider

The term provider registers the scheme `stdio`, which can be used to read and write from the terminal.

Right now it is more of a general i/o interface, since it is not from an actual terminal but raw keys and writes.

## Invokes

### STDIN_MODE
Allows to set the stdin mode:
* `STDIN_MODE_CHARS`: this will transform the keycode to character for you, anything returned from this can be printed (excluding KEY_BACKSPACE). This is the default mode.
* `STDIN_MODE_RAW`: allows to get raw keycodes from the keybaord

## TODO 
Make this alot like actual stdout and stdin
