## Introduction
This repository provides a collection of practical examples for utilizing the Netlink protocol on FreeBSD.

The goal of this project is to offer a clear reference for developers looking to understand the FreeBSD-specific Netlink implementation and transition Linux-based Netlink knowledge to the FreeBSD environment.

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

---

## Example 02

How to register a command, with no attributes (parameters), in the generic netlink family and call it from the userspace.

---

## Example 03

How to register a command, with attributes (parameters), in the generic netlink family and call it from the userspace.
