## Example 01

How to register a new generic netlink family on FreeBSD.

### Build the FreeBSD kernel module

`$ cd example-01/kernel`

`$ make`

### Load the module

`$ cd example-01/kernel`

`$ doas make load`

### Build and run the client

`$ cd example-01/user`

`$ make`

`$ ./nl_echo_client`

### Unload the module

`$ cd example-01/kernel`

`$ doas make unload`
