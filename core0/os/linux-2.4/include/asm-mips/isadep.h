/*
 * Various ISA level dependant constants.
 * Most of the following constants reflect the different layout
 * of Coprocessor 0 registers.
 *
 * Copyright (c) 1998 Harald Koerfgen
 */
#include <linux/config.h>

#ifndef __ASM_ISADEP_H
#define __ASM_ISADEP_H

#if defined(CONFIG_CPU_R3000) || defined(CONFIG_CPU_TX39XX) || defined(CONFIG_CPU_LX4189)
/*
 * R2000 or R3000
 */

/*
 * kernel or user mode? (CP0_STATUS)
 */
#define KU_MASK 0x08
#define	KU_USER 0x08
#define KU_KERN 0x00

#else
/*
 * kernel or user mode?
 */
#define KU_MASK 0x18
#define	KU_USER 0x10
#define KU_KERN 0x00

#endif

#endif /* __ASM_ISADEP_H */
