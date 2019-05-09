# ECHFS provider

The echfs provider registers the scheme `echfs`, which can be used to read from an echfs file system.

Currently there is only read support of the file system, but that includes directory iterations.

## Echfs
The echfs filesystem is a 64-bit FAT-like filesystem which aims to support most UNIX and POSIX-style features while being extremely simple to implement.

For more info see the [echfs github](https://github.com/mintsuki/echfs)

## Invokes

### Read dir
This invoke allows to get the next entry in a dir, if returns false then there are no more entries for the dir.

## TODO

Should eventually implement write support