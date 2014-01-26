#define BME_BASE_ADDR 0xb4410000
#define MAX_ADDR_LINES 0x4
#define PIO_CMD_REG                     (0x00)
#define PIO_ADDR_REG                    (0x01)
#define PIO_DATA_REG                    (0x02)
#define PIO_SCRATCH_REG                 (0x07)
#define COMMAND_PORT                    (0x00)
#define PIO_ADDR_PORT                   (0x01)
#define PIO_DATA_PORT                   (0x02)
#define PIO_WRITE_REG_CMD               (0x00)
#define PIO_READ_REG_CMD                (0x10)
#define DMA_STATUS_PORT                 (0x0a)
#define  _32b  0xffffffff

/* EMI Interface */
#define EMI_MCR(base)   ((base) + 0xe0038000) /* Module Control Register */
#define EMI_RPR(base)   ((base) + 0xe0038004) /* Module Control Register */
#define EMI_RCR(base)   ((base) + 0xe0038008) /* Module Control Register */
#define EMI_PCR(base)   ((base) + 0xe003800c) /* Module Control Register */
#define EMI_SCR(base)   ((base) + 0xe0038010) /* Module Control Register */

  /* writeControlPort macro definition */
  /* writeControlPort macro definition */

void readControlPort(unsigned int offset,unsigned char *data);
void writeControlPort( unsigned int offset,unsigned char val);
void bulkReadControlPort( unsigned int data_addr,unsigned char *pdata,unsigned int numBytes);
void bulkWriteControlPort( unsigned int data_addr,unsigned char *pdata,unsigned int numBytes);
void Rel_cpu(char val);
void reset_BME(void);

/*WESTELL CHANGE*/
void reset_SW_BME(char value);

void clearDmaStatus(unsigned int offset);
void VDSL_LED_ON(void);
void VDSL_LED_OFF(void);
