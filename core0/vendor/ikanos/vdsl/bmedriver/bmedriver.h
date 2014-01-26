#ifndef __BME_DRIVER_H__
#  define __BME_DRIVER_H__
#ifdef __KERNEL__
ssize_t	bme_read(struct file *, char *, size_t, loff_t *);
ssize_t	bme_write(struct file *, const char *, size_t, loff_t *);
int	bme_open(struct inode *, struct file *);
int	bme_release(struct inode *, struct file *);
int	bme_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
extern void    bme_init(void);
#endif


typedef struct read_write_info
{
      unsigned int bme_addr;
      unsigned int bme_offset;
      unsigned char bme_wr_data;
      unsigned char bme_rd_data;
}read_write_info_t;

typedef struct bulk_read_write_info
{
      unsigned int bme_addr;
      unsigned int data_addr;
      unsigned int data_count;
      unsigned char *pdata;
}bulk_read_write_info_t;

#define BME_WRITE              0x1
#define BME_READ               0x2
#define BME_RESET              0x3
#define BME_HOLD_CPU           0x4
#define BME_RELEASE_CPU        0x5
#define BME_CLEAR_DMA_STAUS    0x6
#define BME_LED_ON             0x7
#define BME_LED_OFF            0x8
#define BME_LINE_RATE          0x9
#define BME_BULK_WRITE         0xa
#define BME_BULK_READ          0xb
/*westell change*/
#define BME_SOFT_RESET         0xc
#endif
