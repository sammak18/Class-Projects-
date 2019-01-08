/*
 * tuxctl-ioctl.c
 *
 * Driver (skeleton) for the mp2 tuxcontrollers for ECE391 at UIUC.
 *
 * Mark Murphy 2006
 * Andrew Ofisher 2007
 * Steve Lumetta 12-13 Sep 2009
 * Puskar Naha 2013
 */

#include <asm/current.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/kdev_t.h>
#include <linux/tty.h>
#include <linux/spinlock.h>

#include "tuxctl-ld.h"
#include "tuxctl-ioctl.h"
#include "mtcp.h"

#define debug(str, ...) printk(KERN_DEBUG "%s: " str, __FUNCTION__, ## __VA_ARGS__)

int tux_init(struct tty_struct* tty);
int set_led(struct tty_struct* tty, struct file* file, unsigned cmd, unsigned long arg);
int tux_buttons(struct tty_struct* tty, struct file* file, unsigned cmd, unsigned long arg);

unsigned tux_buttons_values[2];

volatile int local_ack;
static char global_data[6];

/************************ Protocol Implementation *************************/

/* tuxctl_handle_packet()
 * IMPORTANT : Read the header for tuxctl_ldisc_data_callback() in
 * tuxctl-ld.c. It calls this function, so all warnings there apply
 * here as well.
 */

/*
 * IMPORTANT: This function is called from an interrupt context, so it
 *            cannot acquire any semaphores or otherwise sleep, or access
 *            the 'current' pointer. It also must not take up too much time.
*/
void tuxctl_handle_packet (struct tty_struct* tty, unsigned char* packet) {
    unsigned a, b, c;

    a = packet[0]; /* Avoid printk() sign extending the 8-bit */
    b = packet[1]; /* values when printing them. */
    c = packet[2];

    //if(MTCP_ACK==1)
     //   ACK =1;
    switch(a){
        case MTCP_BIOC_EVENT:
            // not the bits recieved from the tux since they are active low
            tux_buttons_values[0]=~b;
            tux_buttons_values[1]=~c;
        case MTCP_RESET:
            // initialise tux and put what was in it, earlier
            tux_init(tty);
            tuxctl_ldisc_put(tty, global_data, 6);
        case MTCP_ACK:
            local_ack= 1;


    }
    /*printk("packet : %x %x %x\n", a, b, c); */
}

/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS packet and the time the 3-byte ACK packet finishes         *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/

int tuxctl_ioctl(struct tty_struct* tty, struct file* file,
                 unsigned cmd, unsigned long arg) {
    switch (cmd) {
        case TUX_INIT:
            return tux_init(tty);
        case TUX_BUTTONS:
            return tux_buttons(tty, file,cmd,arg);

        case TUX_SET_LED:
            if (local_ack==1){
                local_ack=0;
                return set_led( tty, file, cmd, arg);
            }
            else{
                return 0;
            }


        default:
            return -EINVAL;
    }
}

int tux_init(struct tty_struct* tty){
    //printk("enters init");
    char buf[2] = {MTCP_BIOC_ON, MTCP_LED_USR};
    tuxctl_ldisc_put(tty, buf, 2);
    //ACK =1;
    return 0;
}

int tux_buttons(struct tty_struct* tty, struct file* file, unsigned cmd, unsigned long arg){

    unsigned long value,value2,copy,copy2;
    // typecast argument into a pointer
    unsigned long * arg_ptr = (unsigned long * )arg;
/*      Packet format:
 *        Byte 0 - MTCP_BIOC_EVENT
 *        byte 1  +-7-----4-+-3-+-2-+-1-+---0---+
 *            | 1 X X X | C | B | A | START |
 *            +---------+---+---+---+-------+
 *        byte 2  +-7-----4-+---3---+--2---+--1---+-0--+
 *            | 1 X X X | right | down | left | up |
 *            +---------+-------+------+------+----+
 */
    // it has to be: right left down up c b a start
    // putting the eight important bits in value
    value = value & 0x0;
    value = value | (tux_buttons_values[0] & 0x0f);
    value2 = value2 & 0x0;
    value2= value2 | (tux_buttons_values[1] & 0x0f);
    value2 = value2 <<4;
    value = value | value2;


    // right down left up c b a start at the moment, have to swap down and left
    copy = 0x40;
    copy = value & copy;
    // copy has bit for down 
    copy2 = 0x20;
    copy2 = value & copy2;
    // copy2 has bit for down 
    copy = copy>>1;
    copy2 = copy2<<1;

    // delete down and left from value
    value = value & 0x9f;
    
    // copy down and left in correct order
    value = value | copy;
    value = value | copy2;

    // save in memory of arg_ptr
    *arg_ptr = value; 

    return 0;
}

int set_led(struct tty_struct* tty, struct file* file, unsigned cmd, unsigned long arg){
    // arg contains the hex value to be printed and which LEDS are to be turned on;
    // 0000 DDDD 0000 ABCD NUM1 NUM2 NUM3 NUM4
    char data[6];
    // numbers 0 to F in an array: 
    char led_num_arr[16] = {0xE7, 0x06, 0xCB, 0x8F, 0x2E, 0xAD, 0xED, 0x86, 0xEF, 0xAF, 0xEE, 
    0x6D, 0xE1, 0x4F, 0xE9, 0xE8};
    
    // define variables and nessecary masks
    int a,b,c,d,led_on,decimals,i;
    int mask_a,mask_b,mask_c,mask_d,mask_led,mask_dec;
    mask_a = 0x0000000f;  
    mask_b = 0x000000f0;
    mask_c = 0x00000f00;
    mask_d = 0x0000f000;
    mask_led = 0x000f0000;
    mask_dec = 0x0f000000;

    // mask bits in a,b,c,d
    a = arg & mask_a;
    b = arg & mask_b;
    c = arg & mask_c;
    d = arg & mask_d;

    // shift them to appropriate space
    b=b>>4;
    c=c>>8;
    d=d>>12;

    //mask and shift again
    led_on = arg & mask_led;
    led_on = led_on>>16;

    decimals = arg & mask_dec;
    decimals = decimals>>24;

    a = led_num_arr[a];
    b = led_num_arr[b];
    c = led_num_arr[c];
    d = led_num_arr[d];


    // change decimals according to our variable and change in data
    if((decimals & 0x8) == 0x8){
        a= a | 0x10;
    }
    
    if((decimals & 0x4) == 0x4){
        b= b | 0x10;
    }
    
    if((decimals & 0x2) == 0x2){
        c= c | 0X10;

    }

    if((decimals & 0x1) == 0x1){
        d= d | 0x10;
    }

    // data bus to be passed onto tux controller
    data[0] = MTCP_LED_SET;
    data[1] = led_on;
    data[2] = a;
    data[3] = b;
    data[4] = c;
    data[5] = d;
    
    // copy data bus globally to allow for calls
    for(i=0;i<6;i++){
        global_data[i]=data[i];
    }
    // send data to tux
    tuxctl_ldisc_put(tty, data, 6);

    return 0;            
    }
     
/*The argument is a 32-bit integer of the following form: The low 16-bits specify a number whose
hexadecimal value is to be displayed on the 7-segment displays. The low 4 bits of the third byte
specifies which LED’s should be turned on. The low 4 bits of the highest byte (bits 27:24) specify
whether the corresponding decimal points should be turned on.*/
