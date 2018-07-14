/* 
morsecode.c

by Feng Liu - 301218282 - liufengl@sfu.ca
*/


#include <linux/module.h>
#include <linux/miscdevice.h> // for misc-driver calls.
#include <linux/delay.h>	// msleep()
#include <linux/fs.h>
#include <asm/uaccess.h>

//#error Are we building this?

#define MY_DEVICE_FILE  "morse-code"

/**************************************************************
 * FIFO Support
 *************************************************************/
// Info on the interface:
//    https://www.kernel.org/doc/htmldocs/kernel-api/kfifo.html#idp10765104
// Good example:
//    http://lxr.free-electrons.com/source/samples/kfifo/bytestream-example.c
#include <linux/kfifo.h>
#define FIFO_SIZE 256	// Must be a power of 2.
static DECLARE_KFIFO(my_fifo, char, FIFO_SIZE);


/******************************************************
 * Parameter
 ******************************************************/
#define DEFAULT_DOTTIME 200
#define MIN_DOTTIME 0
#define MAX_DOTTIME 2000

#define CHAR_GAP_DOTTIMES 3
#define WORD_GAP_DOTTIMES 7

#define WORD_GAP_NUM_SPACES 3

static int dottime = DEFAULT_DOTTIME;

module_param(dottime, int, S_IRUGO);
MODULE_PARM_DESC(dottime, "Dottime in milliseconds");


/******************************************************
 * LED
 ******************************************************/
#include <linux/leds.h>

DEFINE_LED_TRIGGER(morsecode_ledtrig);

static void led_setEnabled(int isEnabled)
{
	if (isEnabled) {
		led_trigger_event(morsecode_ledtrig, LED_FULL);
	}
	else {
		led_trigger_event(morsecode_ledtrig, LED_OFF);
	}
}

static void led_register(void)
{
	// Setup the trigger's name:
	led_trigger_register_simple("morse-code", &morsecode_ledtrig);
}

static void led_unregister(void)
{
	// Cleanup
	led_trigger_unregister_simple(morsecode_ledtrig);
}


/******************************************************
 * Morse code
 ******************************************************/
#define BITS_PER_BYTE 8

static unsigned short morsecode_codes[] = {
		0xB800,	// A 1011 1
		0xEA80,	// B 1110 1010 1
		0xEBA0,	// C 1110 1011 101
		0xEA00,	// D 1110 101
		0x8000,	// E 1
		0xAE80,	// F 1010 1110 1
		0xEE80,	// G 1110 1110 1
		0xAA00,	// H 1010 101
		0xA000,	// I 101
		0xBBB8,	// J 1011 1011 1011 1
		0xEB80,	// K 1110 1011 1
		0xBA80,	// L 1011 1010 1
		0xEE00,	// M 1110 111
		0xE800,	// N 1110 1
		0xEEE0,	// O 1110 1110 111
		0xBBA0,	// P 1011 1011 101
		0xEEB8,	// Q 1110 1110 1011 1
		0xBA00,	// R 1011 101
		0xA800,	// S 1010 1
		0xE000,	// T 111
		0xAE00,	// U 1010 111
		0xAB80,	// V 1010 1011 1
		0xBB80,	// W 1011 1011 1
		0xEAE0,	// X 1110 1010 111
		0xEBB8,	// Y 1110 1011 1011 1
		0xEEA0	// Z 1110 1110 101
};


/******************************************************
 * Callbacks
 ******************************************************/
static ssize_t my_read(struct file *file,
		char *buff, size_t count, loff_t *ppos)
{
	int num_bytes_read;

	// Pull all available data from fifo into user buffer
	num_bytes_read = 0;
	if (kfifo_to_user(&my_fifo, buff, count, &num_bytes_read)) {
		return -EFAULT;
	}

	return num_bytes_read ;
}

static ssize_t my_write(struct file *file,
		const char *buff, size_t count, loff_t *ppos)
{
	int i, j, k, chIndex, codeEndIndex;
	int numBits;	// Number of bits in our data type for each morse code
	int bitCounter;	// Counts how many bits have been "1" to determine if its a dot or dash
	unsigned short code;	// Morse code for one letter
	unsigned short bit;		// Single bit read from morse code

	numBits = BITS_PER_BYTE * sizeof(unsigned short);

	for (i = 0; i < count; i++) {
		char ch;
		code = 0;

		if (copy_from_user(&ch, &buff[i], sizeof(ch))) {
			return -EFAULT;
		}

		if (ch == ' ') {
			// Sleep for space between words
			msleep(dottime * WORD_GAP_DOTTIMES);

			// Add spaces to the FIFO
			for (j = 0; j < WORD_GAP_NUM_SPACES; j++) {
				if (!kfifo_put(&my_fifo, ' ')) {
					return -EFAULT;
				}
			}
		}
		else if ('A' <= ch && ch <= 'Z') {
			chIndex = ch - 'A';
			code = morsecode_codes[chIndex];
		}
		else if ('a' <= ch && ch <= 'z') {
			chIndex = ch - 'a';
			code = morsecode_codes[chIndex];
		}

		// Blink the LEDs if we have a letter
		if (code) {
			// Find at what index the morse code ends
			for (k = 0; k < numBits; k++) {
				bit = (code & ( 1 << k )) >> k;

				if (bit) {
					codeEndIndex = k;
					break;
				}
			}

			// Read the morse code starting at MSB until the end index
			bitCounter = 0;
			for (k = numBits - 1; k >= codeEndIndex; k--) {
				bit = (code & ( 1 << k )) >> k;

				led_setEnabled(bit);
				msleep(dottime);

				// Insert dash/dot into FIFO by counting how many bits are 1
				if (bit) {
					bitCounter++;
				}
				else {
					if (bitCounter == 1) {
						if (!kfifo_put(&my_fifo, '.')) {
							return -EFAULT;
						}
					}
					else if (bitCounter == 3) {
						if (!kfifo_put(&my_fifo, '-')) {
							return -EFAULT;
						}
					}

					bitCounter = 0;
				}
			}

			// Add the final dot/dash to FIFO
			if (bitCounter == 1) {
				if (!kfifo_put(&my_fifo, '.')) {
					return -EFAULT;
				}
			}
			else if (bitCounter == 3) {
				if (!kfifo_put(&my_fifo, '-')) {
					return -EFAULT;
				}
			}

			// Handle the gap between letters
			led_setEnabled(0);
			msleep(dottime * CHAR_GAP_DOTTIMES);

			if (!kfifo_put(&my_fifo, ' ')) {
				return -EFAULT;
			}
		}
	}


	if (!kfifo_put(&my_fifo, '\n')) {
		return -EFAULT;
	}

	*ppos += count;
	return count;
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

	INIT_KFIFO(my_fifo);

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
