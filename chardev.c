/*
 * chardev.c: Creates a read-only char device that says how many times you've
 * read from the dev file.
 *
 * You can have some fun with this by removing the module_get/put calls,
 * allowing the module to be removed while the file is still open.
 *
 * Compile with `make`. Load with `sudo insmod chardev.ko`. Check `dmesg | tail`
 * output to see the assigned device number and command to create a device file.
 *
 * From TLDP.org's LKMPG book.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/loop.h>  /* for put_user */
MODULE_LICENSE("Dual BSD/GPL");
/*
 * Prototypes - this would normally go in a .h file
 */

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUF_LEN 80

/*
 * Global variables are declared as static, so are global within the file.
 */

static int Major;
static int Device_Open = 0;
static char msg[BUF_LEN];
static char *msg_Ptr;

static struct file_operations fops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

/*
 * This function is called when the module is loaded
 */
int init_module(void)
{
  Major = register_chrdev(0, DEVICE_NAME, &fops);

  if (Major < 0) {
    printk(KERN_ALERT "Registering char device failed with %d\n", Major);
    return Major;
  }

  printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
  printk(KERN_INFO "the driver, create a dev file with\n");
  printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
  printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
  printk(KERN_INFO "the device file.\n");
  printk(KERN_INFO "Remove the device file and module when done.\n");

  return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void)
{
  /*
   * Unregister the device
   */
  unregister_chrdev(Major, DEVICE_NAME);
}

/*
 * Methods
 */

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *filp)
{
  static int counter = 0;

  if (Device_Open)
    return -EBUSY;

  Device_Open++;
  //sprintf(msg,"hello");
 // sprintf( "I already told you %d times Hello world!\n", counter++);
  msg_Ptr = msg;
  /*
   * TODO: comment out the line below to have some fun!
   */
  try_module_get(THIS_MODULE);

  return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *filp)
{

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

	/*
	 * Decrement the usage count, or else once you opened the file, you'll never
	 * get rid of the module.
	 *
	 * TODO: comment out the line below to have some fun!
	 */
	module_put(THIS_MODULE);

	return SUCCESS;
}

/*
 * Called when a process, which already opened the dev file, attempts to read
 * from it.
 */
static ssize_t device_read(struct file *filp, /* see include/linux/fs.h   */
                           char *buffer,      /* buffer to fill with data */
                           size_t length,     /* length of the buffer     */
                           loff_t *offset)
{
  /*
   * Number of bytes actually written to the buffer
   */
  int bytes_read = 0;

  /*
   * If we're at the end of the message, return 0 signifying end of file.
   */
  if (*msg_Ptr == 0)
    return 0;

  /*
   * Actually put the data into the buffer
   */
  while (length && *msg_Ptr) {
    /*
     * The buffer is in the user data segment, not the kernel segment so "*"
     * assignment won't work. We have to use put_user which copies data from the
     * kernel data segment to the user data segment.
     */
    put_user(*(msg_Ptr++), buffer++);
    length--;
    bytes_read++;
  }

  /*
   * Most read functions return the number of bytes put into the buffer
   */
  return bytes_read;
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/hello
 */
	static ssize_t
device_write(struct file *filp,const char *buff, size_t len, loff_t *off)
{
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
   sprintf(msg, buf);   // appending received string with its length
   //size_of_message = strlen(msg);                 // store the length of the stored message
  // printk(KERN_INFO "ourdriver: Received %zu characters from the user\n", len);

   return len;
}
