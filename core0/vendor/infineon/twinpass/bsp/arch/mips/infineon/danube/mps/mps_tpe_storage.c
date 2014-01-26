#include <linux/module.h>
#include <asm/danube/mps_tpe_buffer.h>

static MPS_BUFF_POOL_HEAD *x_mps_buff_pool_array[MPS_MAX_SELECTORS];
MPS_BUFF_POOL_HEAD **mps_buff_pool_array=x_mps_buff_pool_array;
EXPORT_SYMBOL(mps_buff_pool_array);
