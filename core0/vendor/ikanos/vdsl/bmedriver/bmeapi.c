# include <linux/autoconf.h>
# include <linux/kernel.h>
# include <linux/netdevice.h>
# include <linux/types.h>
# include <linux/skbuff.h>
# include <linux/timer.h>
# include <linux/if.h>
# include <linux/if_ether.h>
# include <linux/in.h>
# include <linux/ip.h>
#ifndef LINUX_2_6
# include <linux/compatmac.h>
#endif
#include <linux/delay.h>
#include <asm/io.h>

#include <asm/danube/danube.h>

# include "bmeapi.h"

#define CPLD_VDSL_CHIP_RESET 	*((volatile u16 *)0xB4A00000)
#define VDSL_CHIP_RESET_BIT 0x0001
#define BME_RELEASE_CPU_GPIO 	13
static __u8 *bme_base_addr;

#undef DEBUG

__inline void  bmeGpioSetFlag(volatile u32 *reg, u16 pin, int val)
{
    if (val)
	*(volatile u32 *)reg |= (u32)(0x1 << pin);
    else
	*(volatile u32 *)reg &= (u32)(~(0x1 << pin));
}

void delay( int val);

void delay( int val)
{
    int i;
    for(i=0;i<val;i++);
    return;
}
void timer_ms_wait( int xx)
{

     delay(xx);

}


void reset_BME(void)
{
    CPLD_VDSL_CHIP_RESET = 0xe;
    timer_ms_wait(50000);
    CPLD_VDSL_CHIP_RESET = 0xf;
    timer_ms_wait(50000);
} 

void Rel_cpu(char val)
{
       if(!val)
       {
          bmeGpioSetFlag(DANUBE_GPIO_P0_OUT, BME_RELEASE_CPU_GPIO, 1);
          timer_ms_wait(100);
#ifdef DEBUG
          printk("Rel_cpu() Hold is called %x\n",(1 << (30-16)));
#endif
          return;
       }
       else
       {
          bmeGpioSetFlag(DANUBE_GPIO_P0_OUT, BME_RELEASE_CPU_GPIO, 0);
          timer_ms_wait(100);
#ifdef DEBUG
          printk("Rel_cpu() Release is called %x\n",(1 << (30-16)));
#endif
          return;
       }
}

/*WESTELL CHANGE*/
void reset_SW_BME(char value)
{
    //Rel_cpu(value);
    //timer_ms_wait(1000);
   
    CPLD_VDSL_CHIP_RESET = 0xe;
    timer_ms_wait(50000);
    CPLD_VDSL_CHIP_RESET = 0xf;
    timer_ms_wait(50000);
    bme_init();
  
} 
/*END WESTELL CHANGE*/

static void gpio_config(int port, int pin, int output, int od, int altsel0,
    int altsel1)
{

    switch (port)
    {
    case 0:
	bmeGpioSetFlag(DANUBE_GPIO_P0_DIR, pin, output); /* Output */
	bmeGpioSetFlag(DANUBE_GPIO_P0_OD, pin, od); /* Open drain */

	/* Altsel */
	bmeGpioSetFlag(DANUBE_GPIO_P0_ALTSEL0, pin, altsel0); 
	bmeGpioSetFlag(DANUBE_GPIO_P0_ALTSEL1, pin, altsel1);
	break;
    case 1:
	bmeGpioSetFlag(DANUBE_GPIO_P1_DIR, pin, output); /* Output */
	bmeGpioSetFlag(DANUBE_GPIO_P1_OD, pin, od); /* Open drain */

	/* Altsel */
	bmeGpioSetFlag(DANUBE_GPIO_P1_ALTSEL0, pin, altsel0);
	bmeGpioSetFlag(DANUBE_GPIO_P1_ALTSEL1, pin, altsel1);
	break;
    }
}

void bme_init(void)
{
    /* Memory */
    bme_base_addr = (u8 *)BME_BASE_ADDR;
    if (!bme_base_addr)
	return;
    printk("BME_BASE_ADDR %x->%p\n", BME_BASE_ADDR, bme_base_addr);

#if 0
    /* Note: VDSL is working without changing the bus configuration... */
    *DANUBE_EBU_BUSCON1 |= DANUBE_EBU_BUSCON1_WAIT(0x01);
    printk("EBU_BUSCON1 address is 0x%x \n",DANUBE_EBU_BUSCON1);
#endif

    /* GPIO */
    gpio_config(0, BME_RELEASE_CPU_GPIO, 1, 1, 0, 0);
    gpio_config(1, 10, 0, 1, 1, 0);
    gpio_config(0, 3, 0, 1, 0, 0);
}

void VDSL_LED_ON()
{
    /* Nothing  - LEDs are controled by plf_leds */
}

void VDSL_LED_OFF()
{
    /* Nothing  - LEDs are controled by plf_leds */
}

void writeControlPort(unsigned int offset,unsigned char val)
{
    unsigned short dummy;
    unsigned short _v = (short)val; 
    *(volatile unsigned short *)(bme_base_addr + offset) = _v;
    dummy = CPLD_VDSL_CHIP_RESET;   /* make sure EBU terminates CS after write cycle */
    asm("SYNC");
    timer_ms_wait(200); /* always put in 2 usec delay */
#ifdef DEBUG
    printk("Write Address %x val %02x (%4x)\n",offset,val, _v);
#endif
    return;
}

void bulkWriteControlPort(unsigned int data_addr, unsigned char *pdata, unsigned int numBytes)
{
    unsigned short dummy;
    int i;

#ifdef DEBUG
    printk("Bulk Write Address %x count %d\n",data_addr,numBytes);
#endif
    /* write 32 bit PIO address to address port */
    *(volatile unsigned short *)(bme_base_addr + (PIO_ADDR_PORT << 1)) = (unsigned short)((data_addr & 0xff000000) >> 24);
    dummy = CPLD_VDSL_CHIP_RESET;
    *(volatile unsigned short *)(bme_base_addr + (PIO_ADDR_PORT << 1)) = (unsigned short)((data_addr & 0x00ff0000) >> 16);
    dummy = CPLD_VDSL_CHIP_RESET;
    *(volatile unsigned short *)(bme_base_addr + (PIO_ADDR_PORT << 1)) = (unsigned short)((data_addr & 0x0000ff00) >> 8);
    dummy = CPLD_VDSL_CHIP_RESET;
    *(volatile unsigned short *)(bme_base_addr + (PIO_ADDR_PORT << 1)) = (unsigned short)((data_addr & 0x000000ff));
    dummy = CPLD_VDSL_CHIP_RESET;
    /* write 32 bit PIO data to data port */
    for (i = 0; i < numBytes; i += 4)
    {
        *(volatile unsigned short *)(bme_base_addr + (PIO_DATA_PORT << 1)) = (unsigned short)(*pdata++);
        dummy = CPLD_VDSL_CHIP_RESET;
        *(volatile unsigned short *)(bme_base_addr + (PIO_DATA_PORT << 1)) = (unsigned short)(*pdata++);
        dummy = CPLD_VDSL_CHIP_RESET;
        *(volatile unsigned short *)(bme_base_addr + (PIO_DATA_PORT << 1)) = (unsigned short)(*pdata++);
        dummy = CPLD_VDSL_CHIP_RESET;
        *(volatile unsigned short *)(bme_base_addr + (PIO_DATA_PORT << 1)) = (unsigned short)(*pdata++);
        dummy = CPLD_VDSL_CHIP_RESET;
        /* write to command port */
        *(volatile unsigned short *)(bme_base_addr + (COMMAND_PORT << 1)) = (unsigned short)(PIO_WRITE_REG_CMD);
		asm("SYNC");
        timer_ms_wait(200); /* need 2 usec delay after writing command to control port */
    }
    return;
}

void readControlPort(unsigned int offset,unsigned char *data)
{
    unsigned short _d;
    _d = *((volatile unsigned short *)(bme_base_addr + offset));
    asm("SYNC");
    timer_ms_wait(200);
    *data = _d & 0xff;
#ifdef DEBUG
    printk("Read base_addr %x Offset %x  data %2x (%4x)\n",bme_base_addr,offset,*data, _d);
#endif
    return;
}

void bulkReadControlPort(unsigned int data_addr, unsigned char *pdata, unsigned int numBytes)
{
    unsigned short dummy;
    int i;

#ifdef DEBUG
    printk("Bulk Read Address %x count %d\n",data_addr,numBytes);
#endif
    /* write 32 bit PIO address to address port */
    *(volatile unsigned short *)(bme_base_addr + (PIO_ADDR_PORT << 1)) = (unsigned short)((data_addr & 0xff000000) >> 24);
    dummy = CPLD_VDSL_CHIP_RESET;
    *(volatile unsigned short *)(bme_base_addr + (PIO_ADDR_PORT << 1)) = (unsigned short)((data_addr & 0x00ff0000) >> 16);
    dummy = CPLD_VDSL_CHIP_RESET;
    *(volatile unsigned short *)(bme_base_addr + (PIO_ADDR_PORT << 1)) = (unsigned short)((data_addr & 0x0000ff00) >> 8);
    dummy = CPLD_VDSL_CHIP_RESET;
    *(volatile unsigned short *)(bme_base_addr + (PIO_ADDR_PORT << 1)) = (unsigned short)((data_addr & 0x000000ff));
    dummy = CPLD_VDSL_CHIP_RESET;
    /* read 32 bit PIO data from data port */
    for (i = 0; i < numBytes; i += 4)
    {
        /* read from command port */
        *(volatile unsigned short *)(bme_base_addr + (COMMAND_PORT << 1)) = (unsigned short)(PIO_READ_REG_CMD);
        dummy = CPLD_VDSL_CHIP_RESET;
		asm("SYNC");
        timer_ms_wait(200); /* need 2 usec delay after writing command to control port */
        *pdata++ = (unsigned char)(*(volatile unsigned short *)(bme_base_addr + (PIO_DATA_PORT << 1)));
        dummy = CPLD_VDSL_CHIP_RESET;
        *pdata++ = (unsigned char)(*(volatile unsigned short *)(bme_base_addr + (PIO_DATA_PORT << 1)));
        dummy = CPLD_VDSL_CHIP_RESET;
        *pdata++ = (unsigned char)(*(volatile unsigned short *)(bme_base_addr + (PIO_DATA_PORT << 1)));
        dummy = CPLD_VDSL_CHIP_RESET;
        *pdata++ = (unsigned char)(*(volatile unsigned short *)(bme_base_addr + (PIO_DATA_PORT << 1)));
        dummy = CPLD_VDSL_CHIP_RESET;
    }
    return;
}

void clearDmaStatus(unsigned int offset)
{
     *(volatile unsigned short *)(bme_base_addr + offset) = 0x0000;
     return;
}
