// charenc.c: Creates a char device that encrypts using Caesar Cipher using a key of one.
// Decrypts the cipher text on closing the file.

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/loop.h>
MODULE_LICENSE("Dual BSD/GPL");

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "charenc"
#define BUF_LEN 85

static int Major;
static int Device_Open = 0;
static char msg[BUF_LEN];
static char *msg_Ptr;

// These are the sys calls available for this character driver
static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};


// When the module is loaded using insmod, this function is called
int init_module(void)
{
	// This will give you the Major number for the device
	Major = register_chrdev(0, DEVICE_NAME, &fops);

	if (Major < 0) 
	{
		printk(KERN_ALERT "Registering char device failed with %d\n", Major);
		return Major;
	}

	printk(KERN_INFO " use 'mknod -m 777 /dev/%s c %d 0' to make the device.\n", DEVICE_NAME, Major);
	printk(KERN_INFO "Remove the device file and module when done.\n");

	return SUCCESS;
}


// When the module is unloaded using rmmod , this function is called
void cleanup_module(void)
{
	unregister_chrdev(Major, DEVICE_NAME);
}

//-------------------------------------------------------------------------------------------------------

// When a user program opens the device file this function is called
static int device_open(struct inode *inode, struct file *filp)
{
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	msg_Ptr = msg;

	// Increment the reference count of the module
	try_module_get(THIS_MODULE);

	return SUCCESS;
}

// When a user program closes the device file this function is called
static int device_release(struct inode *inode, struct file *filp)
{
	// Decrypting the cipher text
	char buf[100];
	int i=0;
	while(msg[i] != '\0')
	{
		buf[i]=msg[i];
		if((buf[i]>='a' && buf[i]<='z') || (buf[i]>='A' && buf[i]<='Z'))
		{
			if(buf[i]=='a')
			{
				buf[i]+=25;
			}
			else if(buf[i]=='A')
			{
				buf[i]+=25;
			}
			else
			{
				buf[i]-=1;
			}
		}
		i++;
	}
	buf[i]='\n';
	buf[i+1]='\0';
	sprintf(msg, buf);
	Device_Open--;

	// Decrement the reference count of the module
	module_put(THIS_MODULE);

	return SUCCESS;
}

// When a user program reads from the device file this function is called
static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
	int bytes_read = 0;

	// If we reach the eof we return 0
	if (*msg_Ptr == 0)
		return 0;

	while (length && *msg_Ptr) 
	{
		// put_user transfers data from kernel data segment to the user data segment
		put_user(*(msg_Ptr++), buffer++);
		length--;
		bytes_read++;
	}

	// return the number of bytes read
	return bytes_read;
}

// When a user program writes to the device file this function is called
static ssize_t device_write(struct file *filp,const char *buff, size_t len, loff_t *off)
{
	// Encrypting the plain text
	char buf[100];
	int i=0;
	while(i<len)
	{
		buf[i]=buff[i];
		if((buff[i]>='a' && buff[i]<='z') || (buff[i]>='A' && buff[i]<='Z'))
		{
			if(buf[i]=='z')
			{
				buf[i]-=25;
			}
			else if(buf[i]=='Z')
			{
				buf[i]-=25;
			}
			else
			{
				buf[i]+=1;
			}
		}
		i++;
	}
	buf[i]='\0';
	// writing the encrypted string into the device
	sprintf(msg, buf);

	return len;
}
