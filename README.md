# TODO

- [ ] pdf project report
- [x] source code
- [x] Readme

## Contents of Report

- [ ] Problem Statement
- [ ] Methodolgy
- [ ] Explanation
- [ ] APIs involved/Flow charts if any
- [ ] Screenshots

---

# Simple Caesar Cipher Driver

## To run our Driver use the following commands

 - `make`
	- To compile the driver code

![make]("img/make.png")

 - `insmod charenc.ko`
	- To load the driver to the kernel

![insmod]("img/insmod.png")

 - `dmesg`
	- print or control the kernel message buffer

![dmesg_pt1]("img/demsg_pt1.png")

 - `mknod -m 777 /dev/charenc c <major-number> <minor-number>`
	- This creates a device file with read write and exicute permission

![mkdnod]("img/mknod.png")

## Existency check for driver and device

 - `lsmod`
	- Displays all the drivers loaded to the kernel

![lsmod]("img/lsmod.png")

 - `cat /proc/devices`
	- Displays all the connected devices

![devices]("img/devices.png")

## User process

 - `python userProg.py`
	- Opens the device file and writes "abczf" into the device file
	- Prints the encrypted text
	- Closes the file
 - `cat userProg.py`
	- Displays the source code
 - `cat /dev/charenc`
	- Shows decrypted text (ie. after closing the file)

![userProg]("img/userProg.png")

## Cleanup
 - `rmmod charenc`
	- Unloads the driver
 - `rm /dev/charenc`
	- Remove the device

![cleanup]("img/cleanup.png")

