/*
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 * port.c
 *
 * DANUBE port driver
 *
 */

/* Modification history */
/* 21Jun2004	btxu	Generate from INCA-IP project */


#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>
#include <asm/danube/danube.h>
#include <asm/danube/port.h>
#include "port_defs.h"

#include <rg_config.h>

struct chip_id_rev_map {
	int chip_id;
    char * desc;
};

#ifdef CONFIG_WSTL_WATCHDOG_CPLD
#define CPLD_WDOG_REG ((volatile unsigned short *)(0xB4900000))
#endif
#define PUPSTATE ((u32 volatile *)(0xBF200000))            
#define BOOTMODE ((u32 volatile *)(0xBF200020))            
#define BOOTIP ((u32 volatile *)(0xBF200030))            
#define BOOTMAC ((u32 volatile *)(0xBF200034))            
#define BOOTMAC1 ((u32 volatile *)(0xBF200038))            
#define CMD_REBOOT_WARM        0x44AAFF01
#define CMD_REBOOT_COLD        0x44AAFF04

#ifdef CONFIG_WSTL_USBHUBOVERCURRENT_CPLD
#define CPLD_USBOVERCURRENT_REG ((volatile unsigned short *)(0xB4B00000))
#endif    
#ifdef CONFIG_WSTL_CPLD_CONFIG
#define CPLD_REVISION_REG ((volatile unsigned short *)(0xB4C00000))
#endif

#define MAX_PORTS	2	// Number of ports in system
#define PINS_PER_PORT	16	// Number of available pins per port

static char * bootstr = 0;
static char * bootmode = 0;
static unsigned char bootip[4];
static unsigned char bootmac[6];
static int port_major = 0;
static int danube_port_pin_usage[MAX_PORTS][PINS_PER_PORT]; // Map for pin usage
static u32 danube_port_bases[MAX_PORTS] 
	= {DANUBE_GPIO, \
	   DANUBE_GPIO + 0x00000030}; // Base addresses for ports

static struct semaphore port_sem;

#define WDOG_MODULE_ID 0xF1B0D00D

#define WDOG_PORT   1
#define WDOG_PIN    6

struct timer_list WdogTimer;    /* used to punch the dog */
static unsigned long WdogKickJiffies; /* watchdog kick rate in jiffies */
static struct proc_dir_entry *boot_proc_dir;

/***************************************************************************/
/**                                                                       **/
/** Port usage                                                            **/
/**                                                                       **/
/***************************************************************************/

/**
 *	danube_port_reserve_pin -	Reserve port pin for usage
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function reserves a given pin for usage by the given module.
 *
 *  	Return Value:	
 *	@EBUSY = Pin allready used.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK, pin reserved.
 */
int danube_port_reserve_pin(int port, int pin, int module_id)
{

/*
	if (module_id != PORT_MODULE_ID) {
		if (down_trylock(&port_sem) != 0) return -EBUSY;
	}
*/
	
	if (port < 0 || pin <0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] == 0) {
		danube_port_pin_usage[port][pin] = module_id;
	}
	else {
		return -EBUSY;
	}
	
	return OK;
}

/**
 *	danube_port_free_pin -	Free port pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This functions frees a port pin and thus clears the entry in the 
 *	usage map.
 *
 *	Return Value:	
 *	@EBUSY = Pin used by another module than the given ID.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK, pin freed.
 */
int danube_port_free_pin(int port, int pin, int module_id)
{

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	danube_port_pin_usage[port][pin] = 0;

/*
	if (module_id == PORT_MODULE_ID) {
		up(&port_sem);
	}
*/
	return OK;
}

/**
 *	danube_port_set_open_drain - Enable Open Drain for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function sets Open Drain mode for the given pin.
 *
 *	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_set_open_drain(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_OD_REG,reg);
	reg |= (1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_OD_REG,reg);

	return OK;
}

/**
 *	danube_port_clear_open_drain - Disable Open Drain for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function clears Open Drain mode for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_clear_open_drain(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;

	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_OD_REG,reg);
	reg &= ~(1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_OD_REG,reg);

	return OK;
}

/**
 *	danube_port_set_pudsel - Enable PUDSEL for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function sets the PUDSEL bit for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_set_pudsel(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_PUDSEL_REG,reg);
	reg |= (1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_PUDSEL_REG,reg);

	return OK;
}

/**
 *	danube_port_clear_pudsel - Clear PUDSEL bit for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function clears the PUDSEL bit for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_clear_pudsel(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_PUDSEL_REG,reg);
	reg &= ~(1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_PUDSEL_REG,reg);

	return OK;
}


/**
 *	danube_port_set_puden - Set PUDEN bit for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function sets the PUDEN bit for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_set_puden(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_PUDEN_REG,reg);
	reg |= (1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_PUDEN_REG,reg);

	return OK;
}

/**
 *	danube_port_clear_puden - Disable PUDEN bit for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function clears the PUDEN bit for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_clear_puden(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_PUDEN_REG,reg);
	reg &= ~(1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_PUDEN_REG,reg);

	return OK;
}

/**
 *	danube_port_set_stoff - Enable Schmitt Trigger for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function enables the Schmitt Trigger for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_set_stoff(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_STOFF_REG,reg);
	reg |= (1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_STOFF_REG,reg);

	return OK;
}

/**
 *	danube_port_clear_stoff - Disable Schmitt Trigger for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function disables the Schmitt Trigger for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_clear_stoff(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_STOFF_REG,reg);
	reg &= ~(1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_STOFF_REG,reg);

	return OK;
}

/**
 *	danube_port_set_dir_out - Set direction to output for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function sets the direction for the given pin to output.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_set_dir_out(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_DIR_REG,reg);
	reg |= (1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_DIR_REG,reg);

	return OK;
}

/**
 *	danube_port_set_dir_in - Set direction to input for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function sets the direction for the given pin to input.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_set_dir_in(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_DIR_REG,reg);
	reg &= ~(1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_DIR_REG,reg);

	return OK;
}

/**
 *	danube_port_set_output - Set output bit for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function sets the output bit to 1 for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_set_output(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin <0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_OUT_REG,reg);
	reg |= (1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_OUT_REG,reg);

	return OK;
}

/**
 *	danube_port_clear_output - Clear output bit for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function sets the output bit to 0 for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_clear_output(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_OUT_REG,reg);
	reg &= ~(1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_OUT_REG,reg);

	return OK;
}

/**
 *	danube_port_xor_output - Toggle output bit for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function toggles the output bit for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_toggle_output(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_OUT_REG,reg);
	reg ^= (1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_OUT_REG,reg);

	return OK;
}

/**
 *	danube_port_get_input - Get input bit for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function gets the value of the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@0 = Pin is set to 0
 *	@1 = Pin is set to 1
 */
int danube_port_get_input(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;

	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_IN_REG,reg);
	reg &= (1 << pin);
	if (reg == 0x00) return 0;
	else return 1;
}

/**
 *	danube_port_set_altsel0 - Set alternate select register0 bit for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function sets the alternate select register0 bit to 1 for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_set_altsel0(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin <0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_ALTSEL0_REG,reg);
	reg |= (1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_ALTSEL0_REG,reg);

	return OK;
}


/**
 *	danube_port_clear_altsel0 - Clear alternate select register0 bit for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function sets the alternate select register0 bit to 0 for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_clear_altsel0(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_ALTSEL0_REG,reg);
	reg &= ~(1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_ALTSEL0_REG,reg);

	return OK;
}

/**
 *	danube_port_set_altsel1 - Set alternate select register1 bit for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function sets the alternate select register1 bit to 1 for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */
int danube_port_set_altsel1(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin <0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_ALTSEL1_REG,reg);
	reg |= (1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_ALTSEL1_REG,reg);

	return OK;
}

/**
 *	danube_port_clear_altsel1 - Clear alternate select register1 bit for given pin
 *	@port:      Port number
 *	@pin:       Pin to be reserved
 *	@module_id: Module ID to identify the owner of a pin
 *
 *	This function sets the alternate select register0 bit to 0 for the given pin.
 *
 *  	Return Value:	
 *	@EBUSY = Pin used by another module.
 *	@EINVAL = Invalid port or pin provided.
 *	@OK = OK
 */

int danube_port_clear_altsel1(int port, int pin, int module_id)
{
	u32	reg;

	if (port < 0 || pin < 0) return -EINVAL;
	if (port > MAX_PORTS || pin > PINS_PER_PORT) return -EINVAL;

	if (danube_port_pin_usage[port][pin] != module_id) return -EBUSY;
	
	PORT_READ_REG(danube_port_bases[port] + DANUBE_PORT_ALTSEL1_REG,reg);
	reg &= ~(1 << pin);
	PORT_WRITE_REG(danube_port_bases[port] + DANUBE_PORT_ALTSEL1_REG,reg);

	return OK;
}


/**
 *	danube_port_read_procmem - Create proc file output
 *	@buf:      Buffer to write the string to
 *	@start:    not used (Linux internal)
 *	@offset:   not used (Linux internal)
 *	@count:    not used (Linux internal)
 *	@eof:      Set to 1 when all data is stored in buffer
 *	@data:     not used (Linux internal)
 *
 *	This function creates the output for the port proc file, by reading
 *	all appropriate registers and displaying the content as a table.
 *
 *  	Return Value:	
 *	@len = Lenght of data in buffer
 */
int danube_port_read_procmem(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	long	len = 0;
	int 	t=0;
	u32	bit=0;
	u32	reg=0;

	len = sprintf(buf,"\nDanube Port Settings\n");

	len=len+sprintf(buf+len,"         3         2         1         0\n");
	len=len+sprintf(buf+len,"        10987654321098765432109876543210\n");
	len=len+sprintf(buf+len,"----------------------------------------\n");

	len=len+sprintf(buf+len,"\nP0-OUT: ");
	PORT_READ_REG(DANUBE_GPIO_P0_OUT,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}


	len=len+sprintf(buf+len,"\nP1-OUT: ");
	PORT_READ_REG(DANUBE_GPIO_P1_OUT,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP0-IN:  ");
	PORT_READ_REG(DANUBE_GPIO_P0_IN,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP1-IN:  ");
	PORT_READ_REG(DANUBE_GPIO_P1_IN,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP0-DIR: ");
	PORT_READ_REG(DANUBE_GPIO_P0_DIR,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}


	len=len+sprintf(buf+len,"\nP1-DIR: ");
	PORT_READ_REG(DANUBE_GPIO_P1_DIR,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP0-STO: ");
	PORT_READ_REG(DANUBE_GPIO_P0_STOFF,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP1-STO: ");
	PORT_READ_REG(DANUBE_GPIO_P1_STOFF,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP0-PUDE:");
	PORT_READ_REG(DANUBE_GPIO_P0_PUDEN,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP1-PUDE:");
	PORT_READ_REG(DANUBE_GPIO_P1_PUDEN,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP0-OD:  ");
	PORT_READ_REG(DANUBE_GPIO_P0_OD,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP1-OD:  ");
	PORT_READ_REG(DANUBE_GPIO_P1_OD,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP0-PUDS:");
	PORT_READ_REG(DANUBE_GPIO_P0_PUDSEL,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP1-PUDS:");
	PORT_READ_REG(DANUBE_GPIO_P1_PUDSEL,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP0_ALT0:");
	PORT_READ_REG(DANUBE_GPIO_P0_ALTSEL0,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP0_ALT1:");
	PORT_READ_REG(DANUBE_GPIO_P0_ALTSEL1,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP1_ALT0:");
	PORT_READ_REG(DANUBE_GPIO_P1_ALTSEL0,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}

	len=len+sprintf(buf+len,"\nP1_ALT1:");
	PORT_READ_REG(DANUBE_GPIO_P1_ALTSEL1,reg);
	bit = 0x80000000;
	for (t=0;t<32;t++) {
		if ((reg & bit) > 0) len=len+sprintf(buf+len,"X");
		else len=len+sprintf(buf+len," ");
		bit = bit >> 1;
	}
	
	len=len+sprintf(buf+len,"\n\n");

	*eof = 1;	// No more data available
	return len;
}
#ifdef CONFIG_WSTL_CPLD_CONFIG
/**
 *	danube_port_read_cpld_revision - Create proc file output
 *	@buf:      Buffer to write the string to
 *	@start:    not used (Linux internal)
 *	@offset:   not used (Linux internal)
 *	@count:    not used (Linux internal)
 *	@eof:      Set to 1 when all data is stored in buffer
 *	@data:     not used (Linux internal)
 *
 *	This function reads the CPLD signature bits
 *
 *  	Return Value:	
 *	@len = Lenght of data in buffer
 */
int danube_port_read_cpld_revision(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	long	len = 0;
	u16	rev=0;

    rev = (*CPLD_REVISION_REG & 0x00f0) >> 4;
	len = sprintf(buf,"%d\n", rev);


	*eof = 1;	// No more data available
	return len;
}
#endif
/**
 *	 twinpass_read_chip_id- Create proc file output
 *	@buf:      Buffer to write the string to
 *	@start:    not used (Linux internal)
 *	@offset:   not used (Linux internal)
 *	@count:    not used (Linux internal)
 *	@eof:      Set to 1 when all data is stored in buffer
 *	@data:     not used (Linux internal)
 *
 *	This function reads the MPS_CHIPID register
 *
 *  	Return Value:	
 *	@len = Lenght of data in buffer
 */
int twinpass_read_chip_id(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
    u32 chip_id;
    long	len = 0;

    chip_id = TWINPASS_MPS_CHIPID_VERSION_GET(*(TWINPASS_MPS_CHIPID));
    char *version_str = NULL;

    /*CHIP ID to version description as we know now. */
    static struct  chip_id_rev_map version_map[] ={
        {TWINPASS_REV11, "1.1"},
        {TWINPASS_REV12, "1.2"},
        {TWINPASS_REV13, "1.3"},
        {TWINPASS_REV14, "1.4"},
        {-1, NULL}
    };

   
    int i;
    for (i =0; version_map[i].chip_id != -1; i++ ) {
        if (version_map[i].chip_id ==  chip_id) {
            version_str = version_map[i].desc;
            break;
        }
    }

	len = sprintf(buf,"Twinpass Silicon Chip version %s (CHIP_ID=0x%x) \n", version_str ? version_str : "x.x", chip_id);

	*eof = 1;	// No more data available
	return len;
}
/**
 *	danube_port_read_boot_type - Create proc file output
 *	@buf:      Buffer to write the string to
 *	@start:    not used (Linux internal)
 *	@offset:   not used (Linux internal)
 *	@count:    not used (Linux internal)
 *	@eof:      Set to 1 when all data is stored in buffer
 *	@data:     not used (Linux internal)
 *
 *	This function reads the Reset Status Register
 *
 *  	Return Value:	
 *	@len = Lenght of data in buffer
 */
int danube_port_read_boot_type(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	long	len = 0;

	len = sprintf(buf,"%s\n", bootstr);

	*eof = 1;	// No more data available
	return len;
}

int danube_port_read_boot_mode(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	long	len = 0;

	len = sprintf(buf,"%d\n", bootmode);

	*eof = 1;	// No more data available
	return len;
}

int danube_port_read_boot_ip(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	long	len = 0;

	len = sprintf(buf,"%d.%d.%d.%d\n", bootip[0], bootip[1], bootip[2], bootip[3]);

	*eof = 1;	// No more data available
	return len;
}

int danube_port_read_boot_mac(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	long	len = 0;

	len = sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X\n", bootmac[0], bootmac[1], bootmac[2], bootmac[3], bootmac[4], bootmac[5]);

	*eof = 1;	// No more data available
	return len;
}

static int danube_port_open(struct inode *inode, struct file *filep)
{
#if 0
	if (down_trylock(&port_sem) != 0) return -EBUSY;
#endif

	return OK;
}

static int danube_port_release(struct inode *inode, struct file *filelp)
{
#if 0
	up(&port_sem);
#endif
	
	return OK;
}

static int danube_port_ioctl(struct inode *inode, struct file *filp, 
					unsigned int cmd, unsigned long arg) 
{
	int ret = 0;
	volatile struct danube_port_ioctl_parm parm;
	
	if (_IOC_TYPE(cmd) != DANUBE_PORT_IOC_MAGIC) return -EINVAL;
	
	if (_IOC_DIR(cmd) & _IOC_WRITE) {
		if (!access_ok(VERIFY_READ, arg, sizeof(struct danube_port_ioctl_parm))) 
			return -EFAULT;
		ret=copy_from_user((void*)&parm,(void*)arg,sizeof(struct danube_port_ioctl_parm));
	}
	if (_IOC_DIR(cmd) & _IOC_READ) {
		if (!access_ok(VERIFY_WRITE, arg, sizeof(struct danube_port_ioctl_parm))) 
			return -EFAULT;
	}

	if (down_trylock(&port_sem) != 0) return -EBUSY;

	switch (cmd) {
		case DANUBE_PORT_IOCOD:
			if (parm.value == 0x00) {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_clear_open_drain);
			}
			else {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_set_open_drain);
			}
			break;
		case DANUBE_PORT_IOCPUDSEL:
			if (parm.value == 0x00) {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_clear_pudsel);
			}
			else {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_set_pudsel);
			}
			break;
		case DANUBE_PORT_IOCPUDEN:
			if (parm.value == 0x00) {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_clear_puden);
			}
			else {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_set_puden);
			}
			break;
		case DANUBE_PORT_IOCSTOFF:
			if (parm.value == 0x00) {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_clear_stoff);
			}
			else {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_set_stoff);
			}
			break;
		case DANUBE_PORT_IOCDIR:
			if (parm.value == 0x00) {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_set_dir_in);
			}
			else {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_set_dir_out);
			}
			break;
		case DANUBE_PORT_IOCOUTPUT:
			if (parm.value == 0x00) {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_clear_output);
			}
			else {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_set_output);
			}
			break;
		case DANUBE_PORT_IOCALTSEL0:
			if (parm.value == 0x00) {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_clear_altsel0);
			}
			else {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_set_altsel0);
			}
			break;
		case DANUBE_PORT_IOCALTSEL1:
			if (parm.value == 0x00) {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_clear_altsel1);
			}
			else {
				PORT_IOC_CALL(ret,parm.port,parm.pin,danube_port_set_altsel1);
			}
			break;
		case DANUBE_PORT_IOCINPUT:
			ret=danube_port_reserve_pin(parm.port,parm.pin,PORT_MODULE_ID); 
			if (ret == 0) 
				parm.value=danube_port_get_input(parm.port,parm.pin,PORT_MODULE_ID); 
			ret=danube_port_free_pin(parm.port,parm.pin,PORT_MODULE_ID); 
			copy_to_user((void*)arg,(void*)&parm,sizeof(struct danube_port_ioctl_parm));
			break;
		default:
			ret = -EINVAL;
	}

	up(&port_sem);

	return ret;
}

static struct file_operations port_fops = {
	open    : danube_port_open,
	release : danube_port_release,
	ioctl   : danube_port_ioctl
};

/*
 * This routine runs when the timer expires
 * just kick the dog and restart the timer
 */
static void pet_the_watchdog(void)
{
    /* Restart watchdog */
#ifdef CONFIG_WSTL_WATCHDOG_CPLD
    *CPLD_WDOG_REG ^= 0x01; /* toggle watchdog bit in CPLD */
#else
	danube_port_toggle_output(WDOG_PORT, WDOG_PIN, WDOG_MODULE_ID);
#endif    
#ifdef CONFIG_WSTL_USBHUBOVERCURRENT_CPLD
    {
    unsigned short i;
    if ( 0x07 != (i = *CPLD_USBOVERCURRENT_REG & 0x07))
    {
        printk("WARNING: USB over current port A = %d B= %d C= %d\n", i & 1, (i & 2) >> 1, (i & 4) >> 2); 
    }
    }
#endif    
    WdogTimer.expires = jiffies + WdogKickJiffies;
    add_timer(&WdogTimer);

}

/**
 *	danube_port_init - Initialize port structures
 *
 *	This function initializes the internal data structures of the driver
 *	and will create the proc file entry and device.
 *
 *  	Return Value:	
 *	@OK = OK
 */
int __init danube_port_init(void)
{
	int t = 0;
	int i = 0;
	int err = 0;
    u32 data;
/*	u32 pins = 0; */

	printk("Danube Port Initialization\n");

	sema_init(&port_sem,1);

    *DANUBE_EBU_BUSCON1 = 0x4041D3D5; /* CPLD access timing with extended address setup */

    data = *PUPSTATE;
    if (CMD_REBOOT_COLD == data)
        bootstr = "cold";
    else if (CMD_REBOOT_WARM == data)
        bootstr = "warm";
    else
        bootstr = "unknown";

    printk("%s boot\n", bootstr);

    data = *BOOTMODE;
    if ((data & 0xFFFFFF00) != 0x44AA8800)
    {
        bootmode = 0;
        bootip[0] = 0;
        bootip[1] = 0;
        bootip[2] = 0;
        bootip[3] = 0;
        bootmac[0] = 0;
        bootmac[1] = 0;
        bootmac[2] = 0;
        bootmac[3] = 0;
        bootmac[4] = 0;
        bootmac[5] = 0;
    }
    else
    {
        bootmode = (*BOOTMODE) & 0xff;
        data = *BOOTIP;
        bootip[0] = (data >> 24) & 0xff;
        bootip[1] = (data >> 16) & 0xff;
        bootip[2] = (data >>  8) & 0xff;
        bootip[3] = data & 0xff;
        data = *BOOTMAC;
        bootmac[0] = (data >> 24) & 0xff;
        bootmac[1] = (data >> 16) & 0xff;
        bootmac[2] = (data >>  8) & 0xff;
        bootmac[3] = (data) & 0xff;
        data = *BOOTMAC1;
        bootmac[4] = (data >> 24) & 0xff;
        bootmac[5] = (data >> 16) & 0xff;
    }
/*	
	for (t = 0; t<MAX_PORTS; t++) {
		for (i = 0;i<PINS_PER_PORT;i++) danube_port_pin_usage[t][i] = -1;
	}
*/

	/* Check ports for available pins */
	for (t = 0; t<MAX_PORTS; t++) {
/*
		pins = danube_mux_gpio_pins(t);
		for (i = 0;i<PINS_PER_PORT;i++) {
			if ((pins & 0x00000001) > 0) 
				danube_port_pin_usage[t][i] = 0;
			pins = pins >> 1;
		}
*/
		for (i = 0;i<PINS_PER_PORT;i++)
				danube_port_pin_usage[t][i] = 0;
	}
	
   	/* register port device */
   	err = register_chrdev(0,"danube-port", &port_fops);
	if (err > 0) port_major = err;
	else {
		printk("danube-port: Error! Could not register port device. #%d\n",err);
		return err;
	}
	
	/* Create proc file */ 
	create_proc_read_entry("driver/danube_port", 0, NULL, danube_port_read_procmem, NULL);
#ifdef CONFIG_WSTL_CPLD_CONFIG
    create_proc_read_entry("revision", 0, proc_mkdir("driver/cpld", NULL), danube_port_read_cpld_revision, NULL);
#endif
	create_proc_read_entry("driver/boottype", 0, NULL, danube_port_read_boot_type, NULL);
    boot_proc_dir = proc_mkdir("driver/boot", NULL);
    create_proc_read_entry("type", 0, boot_proc_dir, danube_port_read_boot_type, NULL);
    create_proc_read_entry("mode", 0, boot_proc_dir, danube_port_read_boot_mode, NULL);
    create_proc_read_entry("ip", 0, boot_proc_dir, danube_port_read_boot_ip, NULL);
    create_proc_read_entry("mac", 0, boot_proc_dir, danube_port_read_boot_mac, NULL);

    /*For CHIP ID VERSION, debugging*/
    create_proc_read_entry("version", 0, proc_mkdir("chipid", NULL), twinpass_read_chip_id, NULL);
#ifdef CONFIG_WSTL_WATCHDOG_CPLD
    /* no initialization needed just toggle the bit */
    *CPLD_WDOG_REG ^= 0x01; /* toggle watchdog bit in CPLD */
#else
    /*
     * hardware watchdog GPIO initialization code
     */
	danube_port_reserve_pin(WDOG_PORT, WDOG_PIN, WDOG_MODULE_ID);
	danube_port_set_dir_out(WDOG_PORT, WDOG_PIN, WDOG_MODULE_ID);
	danube_port_clear_altsel0(WDOG_PORT, WDOG_PIN, WDOG_MODULE_ID);
	danube_port_clear_altsel1(WDOG_PORT, WDOG_PIN, WDOG_MODULE_ID);

	danube_port_toggle_output(WDOG_PORT, WDOG_PIN, WDOG_MODULE_ID);
#endif
    WdogKickJiffies = (HZ)/10;  /* kick every 100 milliseconds */
    init_timer(&WdogTimer);
    WdogTimer.data = 0;
    WdogTimer.function = pet_the_watchdog;
    WdogTimer.expires = jiffies + WdogKickJiffies;
    add_timer(&WdogTimer);  /* get the kick timer queued */


    printk("Watchdog started Kick jiffies = %d\n", WdogKickJiffies);

#ifdef CONFIG_WSTL_USBHUBOVERCURRENT_CPLD
    printk("USB over current monitoring active\n");
#endif    
	return OK;
}

module_init(danube_port_init);

EXPORT_SYMBOL(danube_port_reserve_pin);
EXPORT_SYMBOL(danube_port_free_pin);
EXPORT_SYMBOL(danube_port_set_open_drain);
EXPORT_SYMBOL(danube_port_clear_open_drain);
EXPORT_SYMBOL(danube_port_set_pudsel);
EXPORT_SYMBOL(danube_port_clear_pudsel);
EXPORT_SYMBOL(danube_port_set_puden);
EXPORT_SYMBOL(danube_port_clear_puden);
EXPORT_SYMBOL(danube_port_set_stoff);
EXPORT_SYMBOL(danube_port_clear_stoff);
EXPORT_SYMBOL(danube_port_set_dir_out);
EXPORT_SYMBOL(danube_port_set_dir_in);
EXPORT_SYMBOL(danube_port_set_output);
EXPORT_SYMBOL(danube_port_clear_output);
EXPORT_SYMBOL(danube_port_toggle_output);
EXPORT_SYMBOL(danube_port_get_input);

EXPORT_SYMBOL(danube_port_set_altsel0);
EXPORT_SYMBOL(danube_port_clear_altsel0);
EXPORT_SYMBOL(danube_port_set_altsel1);
EXPORT_SYMBOL(danube_port_clear_altsel1);


