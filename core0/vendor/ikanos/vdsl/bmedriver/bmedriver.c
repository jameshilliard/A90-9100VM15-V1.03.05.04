/*
 * linux/drivers/char/bmedrv.c
 *
 * Front-end raw character devices.  These can be bound to any block
 * devices to provide genuine Unix raw character device semantics.
 *
 * We reserve minor number 0 for a control interface.  ioctl()s on this
 * device are used to bind the other minor numbers to block devices.
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/delay.h>
#ifndef CONFIG_KERNELVERSION
#include<linux/devfs_fs_kernel.h>
#endif
#include <asm/uaccess.h>
#include "bmedriver.h"
#include "bmeapi.h"

unsigned int vdslUsRate,vdslDsRate;

static struct file_operations bme_fops = {
	read:		bme_read,
	write:		bme_write,
	open:		bme_open,
	ioctl:		bme_ioctl,
	release:	bme_release,
};

static int bme_major = BME_MAJOR;
static unsigned char xfer_data[256];
#ifdef LINUX_2_6
static struct class_simple *bme_class;

int __init bmedrv_init(void)
{
        int retval;

	printk("Registering bme with major %d\n", bme_major);
        retval = register_chrdev(bme_major,"bme",&bme_fops);
#if 0
#ifdef CONFIG_KERNELVERSION
/* for linux-2.6.18 kernel */
        if (bme_major > 0) {
                bme_class = class_create(THIS_MODULE, "bme");
                if (IS_ERR(bme_class)) {
                        retval = PTR_ERR(bme_class);
                        goto out_chrdev;
                }
                class_device_create(bme_class, NULL, MKDEV(bme_major, 0),NULL, "bme");
                printk("***** VDSL BME driver registered with Major number: %d *****\n",bme_major);
        }
#else
/* for linux-mips-2.6.12 kernel */
        if (bme_major > 0) {
                bme_class = class_simple_create(THIS_MODULE, "bme");
                if (IS_ERR(bme_class)) {
                        retval = PTR_ERR(bme_class);
                        goto out_chrdev;
                }
                class_simple_device_add(bme_class, MKDEV(bme_major, 0), NULL, "bme");
                retval = devfs_mk_cdev(MKDEV(bme_major, 0), S_IFCHR|S_IRUSR|S_IWUSR, "bme");
                if (retval)
                        goto out_class;
        }
#endif
#endif
        if (ret < 0)
        {
          printk("failed to get a mojor number for BME device: %d\n",bme_major);
          return bme_major;
        }

        bme_init();
        return 0;

out:
        if(retval)
        {
           printk("failed to register BME character device. ERROR: %d\n", retval);
        }
        return retval;

#ifndef CONFIG_KERNELVERSION
/* for linux-mips-2.6.12 kernel */
out_class:
        class_simple_device_remove(MKDEV(bme_major,0));
        class_simple_destroy(bme_class);
#endif
out_chrdev:
        unregister_chrdev(bme_major, "bme");
        goto out;
}

#else
int __init bmedrv_init(void)
{

    int retval;

    printk("Registering bme with major %d\n", bme_major);
    if ((retval = register_chrdev(bme_major, "bme", &bme_fops)) < 0)
    {
	printk("failed to get a mojor number for BME device: %d\n",bme_major);
	return retval;
    }

    bme_init(); 
    return 0;
}
#endif

static void bme_exit(void)
{
       if (unregister_chrdev(bme_major, "bme") != 0)
               printk(KERN_ERR "BME: failed to unregister bme device\n");
#ifdef LINUX_2_6
#ifdef CONFIG_KERNELVERSION
/* for linux-2.6.18 kernel */
       class_device_destroy(bme_class, MKDEV(bme_major, 0));
       class_destroy(bme_class);
#else
       devfs_remove("bme");
       class_simple_device_remove(MKDEV(bme_major, 0));
       class_simple_destroy(bme_class);
#endif
       printk("Ikanos BME is now unregisterd\n"); 
#endif
#ifdef DEBUG
       printk("Ikanos BME is now unregisterd\n"); 
#endif
       return;
}
/* 
 * Open/close code for raw IO.
 */

int bme_open(struct inode *inode, struct file *filp)
{
#ifdef DEBUG
       printk("Ikanos BME Open called\r\n");
#endif
       return 0;
}

int bme_release(struct inode *inode, struct file *filp)
{
#ifdef DEBUG
      printk("Ikanos BME Release called\r\n");
#endif
      return 0;
}


/*
 * Deal with ioctls against the raw-device control interface, to bind
 * and unbind other raw devices.  
 */
int bme_ioctl(struct inode *inode, 
		  struct file *flip,
		  unsigned int command, 
		  unsigned long arg)
{

     unsigned int flags;
     read_write_info_t data;
     bulk_read_write_info_t bulkdata;

#ifdef DEBUG
    printk("Ikanos BME IOCTL called cmd %x %x\r\n",command, arg);
#endif

    switch(command)
    {
        case BME_READ :
#ifdef LINUX_2_6
            local_irq_save(flags);
#else
            save_and_cli(flags);
#endif
#ifdef DEBUG
            printk("read  \r\n");
#endif
            if (copy_from_user(&data, (read_write_info_t *)arg, 
                                              sizeof(read_write_info_t)))
            {
                 printk("handle_read: copy_from_user failed\n");
                 return -EFAULT;
            }
            readControlPort (data.bme_offset<<1,
                                    (unsigned char *)&(data.bme_rd_data));
     
            if (copy_to_user((read_write_info_t *)arg, &data, 
                                              sizeof(read_write_info_t)))
            {
                printk(" handle_read: copy_to_user failed\n");
                return -EFAULT;
            }
#ifdef LINUX_2_6
            local_irq_restore(flags);
#else
            restore_flags(flags);
#endif
            break;
        case BME_WRITE :
#ifdef LINUX_2_6
            local_irq_save(flags);
#else
            save_and_cli(flags);
#endif
#ifdef DEBUG
            printk("write  %d\r\n",wrCnt);
#endif
            if (copy_from_user(&data, (read_write_info_t *)arg, 
                                            sizeof(read_write_info_t)))
            {
                 printk("handle_write: copy_from_user failed\n");
                 return -EFAULT;
            }
            writeControlPort ((data.bme_offset<<1),
                                                     data.bme_wr_data);
#ifdef LINUX_2_6
            local_irq_restore(flags);
#else
            restore_flags(flags);
#endif
            break;
    case BME_BULK_READ :
#ifdef LINUX_2_6
        local_irq_save(flags);
#else
        save_and_cli(flags);
#endif
        if (copy_from_user(&bulkdata, (bulk_read_write_info_t *)arg, 
                                        sizeof(bulk_read_write_info_t)))
        {
             printk("handle_bulkread: copy_from_user failed\n");
             return -EFAULT;
        }
        if (bulkdata.data_count > 256)    /* 256 is max there can be */
        {
             printk("handle_bulkreaddata: data transfer count %d > 256\n", bulkdata.data_count);
             return -EFAULT;
        }
        bulkReadControlPort (bulkdata.data_addr, &xfer_data[0], bulkdata.data_count);
        if (copy_to_user(bulkdata.pdata, &xfer_data[0], bulkdata.data_count))
        {
             printk("handle_bulkreaddata: copy_to_user failed\n");
             return -EFAULT;
        }
#ifdef LINUX_2_6
        local_irq_restore(flags);
#else
        restore_flags(flags);
#endif
        break;
    case BME_BULK_WRITE :
#ifdef LINUX_2_6
        local_irq_save(flags);
#else
        save_and_cli(flags);
#endif
        if (copy_from_user(&bulkdata, (bulk_read_write_info_t *)arg, 
                                        sizeof(bulk_read_write_info_t)))
        {
             printk("handle_bulkwrite: copy_from_user failed\n");
             return -EFAULT;
        }
        if (bulkdata.data_count > 256)    /* 256 is max there can be */
        {
             printk("handle_bulkwritedata: data transfer count %d > 256\n", bulkdata.data_count);
             return -EFAULT;
        }
        if (copy_from_user(&xfer_data[0], bulkdata.pdata, bulkdata.data_count))
        {
             printk("handle_bulkwritedata: copy_from_user failed\n");
             return -EFAULT;
        }
        bulkWriteControlPort (bulkdata.data_addr, &xfer_data[0], bulkdata.data_count);
#ifdef LINUX_2_6
        local_irq_restore(flags);
#else
        restore_flags(flags);
#endif
        break;
        case BME_HOLD_CPU :
#ifdef LINUX_2_6
            local_irq_save(flags);
#else
            save_and_cli(flags);
#endif
#ifdef DEBUG
             printk("Kernel Holding cpu  \r\n");
#endif
            Rel_cpu(0);
#ifdef LINUX_2_6
            local_irq_restore(flags);
#else
            restore_flags(flags);
#endif
            break;
        case BME_RELEASE_CPU :
#ifdef LINUX_2_6
            local_irq_save(flags);
#else
            save_and_cli(flags);
#endif
#ifdef DEBUG
            printk("Kernel Rel cpu  \r\n");
#endif
            Rel_cpu(1);
#ifdef LINUX_2_6
            local_irq_restore(flags);
#else
            restore_flags(flags);
#endif
            break;
        case BME_RESET :
#ifdef LINUX_2_6
            local_irq_save(flags);
#else
            save_and_cli(flags);
#endif
#ifdef DEBUG
            printk("Kernel Reset BME  \r\n");
#endif
            reset_BME();
#ifdef LINUX_2_6
            local_irq_restore(flags);
#else
            restore_flags(flags);
#endif
            break;
        case BME_CLEAR_DMA_STAUS :
#ifdef LINUX_2_6
            local_irq_save(flags);
#else
            save_and_cli(flags);
#endif
#ifdef DEBUG
            printk("Kernel clearDmaStatus  \r\n");
#endif
            clearDmaStatus(0xa<<2);
#ifdef LINUX_2_6
            local_irq_restore(flags);
#else
            restore_flags(flags);
#endif
             break;
       case BME_LED_ON :
#ifdef LINUX_2_6
            local_irq_save(flags);
#else
            save_and_cli(flags);
#endif
#ifdef DEBUG
            printk("Kernel LED ON  \r\n");
#endif
            VDSL_LED_ON();
#ifdef LINUX_2_6
            local_irq_restore(flags);
#else
            restore_flags(flags);
#endif
             break;
        case BME_LED_OFF :
#ifdef LINUX_2_6
            local_irq_save(flags);
#else
            save_and_cli(flags);
#endif
#ifdef DEBUG
            printk("Kernel LED OFF  \r\n");
#endif
            VDSL_LED_OFF();
#ifdef LINUX_2_6
            local_irq_restore(flags);
#else
            restore_flags(flags);
#endif
             break;
        case BME_LINE_RATE :
#ifdef LINUX_2_6
            local_irq_save(flags);
#else
            save_and_cli(flags);
#endif
            if (copy_from_user(&data, (read_write_info_t *)arg,
                                              sizeof(read_write_info_t)))
            {
                 printk("handle_read: copy_from_user failed\n");
                 return -EFAULT;
            }
#ifdef DEBUG
            printk("Kernel LED OFF  \r\n");
            printk("VDSL Up Rate %d kbps Down Rate %d kbps \n",data.bme_addr,
		data.bme_offset) ;
#endif
            vdslUsRate = data.bme_addr;
            vdslDsRate = data.bme_offset;
#ifdef LINUX_2_6
            local_irq_restore(flags);
#else
            restore_flags(flags);
#endif
             break;
/*WESTELL CHANGE*/
              case BME_SOFT_RESET :
#ifdef LINUX_2_6
            local_irq_save(flags);
#else
            save_and_cli(flags);
#endif
//#ifdef DEBUG
            printk("Kernel SW Reset BME  \r\n");
//#endif
            reset_SW_BME(0);
#ifdef LINUX_2_6
            local_irq_restore(flags);
#else
            restore_flags(flags);
#endif
            break;
/*END WESTELL CHANGE*/
        default:
             printk("BME Unknown ioctl cmd %x..\n",command); 
             return -1;
    }
    return 0;
}

int bme_read(struct file *filp, char * buf, 
		 size_t size, loff_t *offp)
{
#ifdef DEBUG
       printk("Ikanos BME Read called\r\n");
       strncpy(buf,"Hello Read",11);
#endif
       return 0;
}

int bme_write(struct file *filp, const char *buf, 
		  size_t size, loff_t *offp)
{
#ifdef DEBUG
       printk("Ikanos BME Write called\r\n");
       printk("Ikanos  Write : %s\r\n",buf);
#endif
       return 0;
}

#ifdef LINUX_2_6
EXPORT_SYMBOL(vdslUsRate);
EXPORT_SYMBOL(vdslDsRate);
#endif

module_init(bmedrv_init);
module_exit(bme_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Linux Router Team ");
MODULE_DESCRIPTION("Driver Interface to the BME");
