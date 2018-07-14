/* 
morsecode.c

by Feng Liu - 301218282 - liufengl@sfu.ca
*/


#include <linux/module.h>
#include <linux/miscdevice.h> // for misc-driver calls.
#include <linux/delay.h>	// msleep()
#include <linux/fs.h>

//#error Are we building this?

#define MY_DEVICE_FILE  "corse_code"

/******************************************************
 * Parameter
 ******************************************************/
#define DEFAULT_DOTTIME 200
#define MIN_DOTTIME 0
#define MAX_DOTTIME 2000

static int dottime = DEFAULT_DOTTIME;

module_param(dottime, int, S_IRUGO);
MODULE_PARM_DESC(dottime, "Dottime in milliseconds");


/******************************************************
 * LED
 ******************************************************/
#include <linux/leds.h>

DEFINE_LED_TRIGGER(ledtrig_demo);

#define LED_ON_TIME_ms 100
#define LED_OFF_TIME_ms 900

static void my_led_blink(void)
{
	led_trigger_event(ledtrig_demo, LED_FULL);
	msleep(LED_ON_TIME_ms);
	led_trigger_event(ledtrig_demo, LED_OFF);
	msleep(LED_OFF_TIME_ms);
}

static void led_register(void)
{
	// Setup the trigger's name:
	led_trigger_register_simple("demo", &ledtrig_demo);
}

static void led_unregister(void)
{
	// Cleanup
	led_trigger_unregister_simple(ledtrig_demo);
}


/******************************************************
 * Callbacks
 ******************************************************/
static ssize_t my_read(struct file *file,
		char *buff, size_t count, loff_t *ppos)
{
	my_led_blink();
	return 0;
}

static ssize_t my_write(struct file *file,
		const char *buff, size_t count, loff_t *ppos)
{
	return 0;
}


/******************************************************
 * Misc support
 ******************************************************/
// Callbacks:  (structure defined in /linux/fs.h)
struct file_operations my_fops = {
	.owner    =  THIS_MODULE,
	.read     =  my_read,
	.write    =  my_write,
};

// Character Device info for the Kernel:
static struct miscdevice my_miscdevice = {
		.minor    = MISC_DYNAMIC_MINOR,         // Let the system assign one.
		.name     = MY_DEVICE_FILE,             // /dev/.... file.
		.fops     = &my_fops                    // Callback functions.
};


/******************************************************
 * Driver initialization and exit:
 ******************************************************/
static int __init morsedriver_init(void)
{
	int ret;
	
	printk(KERN_INFO "----> Morse code driver init()\n");

	if (dottime < MIN_DOTTIME || MAX_DOTTIME < dottime) {
		printk(KERN_ERR "Morse code dottime must be between [%d] and [%d]!\n", MIN_DOTTIME, MAX_DOTTIME);
	}

	// Register as a misc driver:
	ret = misc_register(&my_miscdevice);

	led_register();

	return ret;
}

static void __exit morsedriver_exit(void)
{
	printk(KERN_INFO "<---- Morse code driver exit().\n");

	// Unregister misc driver
	misc_deregister(&my_miscdevice);

	led_unregister();
}

// Link our init/exit functions into the kernel's code.
module_init(morsedriver_init);
module_exit(morsedriver_exit);

// Information about this module:
MODULE_AUTHOR("Eric Liu");
MODULE_DESCRIPTION("A morse code driver for as4");
MODULE_LICENSE("GPL");
