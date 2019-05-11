# Terminal provider

The term provider registers the scheme `term`, which can be used to write to the terminal, so basically like stdout.

## TODO

This should probably be something like `stdio://<type>/`. with the types being:
* stdout
* stderr
* stdin

and have that the default one (`stdio:///`) will act as both stdout and stdin
