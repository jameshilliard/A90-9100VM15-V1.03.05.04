/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1995, 1996 by Ralf Baechle, Andreas Busse,
 *                             Stoned Elipot and Paul M. Antoine.
 */
#ifndef _ASM_BOOTINFO_H
#define _ASM_BOOTINFO_H

#include <linux/types.h>

/*
 * Values for machgroup
 */
#define MACH_GROUP_UNKNOWN      0 /* whatever...                            */
#define MACH_GROUP_JAZZ     	1 /* Jazz                                   */
#define MACH_GROUP_DEC          2 /* Digital Equipment                      */
#define MACH_GROUP_ARC		3 /* Wreckstation Tyne, rPC44, possibly other */
#define MACH_GROUP_SNI_RM	4 /* Siemens Nixdorf RM series              */
#define MACH_GROUP_ACN		5
#define MACH_GROUP_SGI          6 /* Silicon Graphics                       */
#define MACH_GROUP_COBALT       7 /* Cobalt servers		 	    */
#define MACH_GROUP_NEC_DDB	8 /* NEC DDB                                */
#define MACH_GROUP_BAGET	9 /* Baget                                  */
#define MACH_GROUP_COSINE      10 /* CoSine Orion                           */
#define MACH_GROUP_GALILEO     11 /* Galileo Eval Boards                    */
#define MACH_GROUP_MOMENCO     12 /* Momentum Boards                        */
#define MACH_GROUP_ITE         13 /* ITE Semi Eval Boards                   */
#define MACH_GROUP_PHILIPS     14
#define MACH_GROUP_GLOBESPAN   15 /* Globespan PVR Referrence Board         */
#define MACH_GROUP_SIBYTE      16 /* Sibyte Eval Boards                     */
#define MACH_GROUP_TOSHIBA     17 /* Toshiba Reference Systems TSBREF       */
#define MACH_GROUP_ALCHEMY     18 /* Alchemy Semi Eval Boards               */
#define MACH_GROUP_NEC_VR41XX  19 /* NEC Vr41xx based boards/gadgets        */
#define MACH_GROUP_HP_LJ       20 /* Hewlett Packard LaserJet               */
#define MACH_GROUP_LASAT       21
#define MACH_GROUP_BRCM        22 /* Broadcom boards                          */
#define MACH_GROUP_INCAIP      23 /* INCAIP */
#define MACH_GROUP_ADM_GW      24 /* ADMTEK */
#define MACH_GROUP_ANALOG      25 /* Analog Devices Boards */
#define MACH_GROUP_AMAZON      26 /* Infineon AMAZON                        */
#define MACH_GROUP_DANUBE      27 /* Infineon DANUBE                        */
#define MACH_GROUP_TWINPASS    28 /* Infineon TWINPASS (-VE or -E)          */

/*
 * Valid machtype values for group unknown (low order halfword of mips_machtype)
 */
#define MACH_UNKNOWN		0	/* whatever...			*/

/*
 * Valid machtype values for group JAZZ
 */
#define MACH_ACER_PICA_61	0	/* Acer PICA-61 (PICA1)		*/
#define MACH_MIPS_MAGNUM_4000	1	/* Mips Magnum 4000 "RC4030"	*/
#define MACH_OLIVETTI_M700      2	/* Olivetti M700-10 (-15 ??)    */

/*
 * Valid machtype for group DEC
 */
#define MACH_DSUNKNOWN		0
#define MACH_DS23100		1	/* DECstation 2100 or 3100	*/
#define MACH_DS5100		2	/* DECsystem 5100		*/
#define MACH_DS5000_200		3	/* DECstation 5000/200		*/
#define MACH_DS5000_1XX		4	/* DECstation 5000/120, 125, 133, 150 */
#define MACH_DS5000_XX		5	/* DECstation 5000/20, 25, 33, 50 */
#define MACH_DS5000_2X0		6	/* DECstation 5000/240, 260	*/
#define MACH_DS5400		7	/* DECsystem 5400		*/
#define MACH_DS5500		8	/* DECsystem 5500		*/
#define MACH_DS5800		9	/* DECsystem 5800		*/
#define MACH_DS5900		10	/* DECsystem 5900		*/

/*
 * Valid machtype for group ARC
 */
#define MACH_DESKSTATION_RPC44  0	/* Deskstation rPC44 */
#define MACH_DESKSTATION_TYNE	1	/* Deskstation Tyne */

/*
 * Valid machtype for group SNI_RM
 */
#define MACH_SNI_RM200_PCI	0	/* RM200/RM300/RM400 PCI series */

/*
 * Valid machtype for group ACN
 */
#define MACH_ACN_MIPS_BOARD	0       /* ACN MIPS single board        */

/*
 * Valid machtype for group SGI
 */
#define MACH_SGI_IP22		0	/* Indy, Indigo2, Challenge S */
#define MACH_SGI_IP27		1	/* Origin 200, Origin 2000, Onyx 2 */
#define MACH_SGI_IP28		2	/* Indigo2 Impact */
#define MACH_SGI_IP32		3	/* O2 */

/*
 * Valid machtype for group COBALT
 */
#define MACH_COBALT_27		0	/* Proto "27" hardware */

/*
 * Valid machtype for group NEC DDB
 */
#define MACH_NEC_DDB5074	0	/* NEC DDB Vrc-5074 */
#define MACH_NEC_DDB5476	1	/* NEC DDB Vrc-5476 */
#define MACH_NEC_DDB5477	2	/* NEC DDB Vrc-5477 */
#define MACH_NEC_ROCKHOPPER	3	/* Rockhopper base board */
#define MACH_NEC_ROCKHOPPERII	4	/* Rockhopper II base board */

/*
 * Valid machtype for group BAGET
 */
#define MACH_BAGET201		0	/* BT23-201 */
#define MACH_BAGET202		1	/* BT23-202 */

/*
 * Cosine boards.
 */
#define MACH_COSINE_ORION	0

/*
 * Valid machtype for group GALILEO
 */
#define MACH_EV96100		0	/* EV96100 */
#define MACH_EV64120A		1	/* EV64120A */

/*
 * Valid machtype for group MOMENCO
 */
#define MACH_MOMENCO_OCELOT		0
#define MACH_MOMENCO_OCELOT_G		1
#define MACH_MOMENCO_OCELOT_C		2

/*
 * Valid machtype for group ITE
 */
#define MACH_QED_4N_S01B	0	/* ITE8172 based eval board */

/*
 * Valid machtype for group Globespan
 */
#define MACH_IVR		0	/* IVR eval board */

/*
 * Valid machtype for group PHILIPS
 */
#define MACH_PHILIPS_NINO	0	/* Nino */
#define MACH_PHILIPS_VELO	1	/* Velo */

/*
 * Valid machtype for group SIBYTE
 */
#define MACH_SWARM              0

/*
 * Valid machtypes for group Toshiba
 */
#define MACH_PALLAS		0
#define MACH_TOPAS		1
#define MACH_JMR		2
#define MACH_TOSHIBA_JMR3927	3	/* JMR-TX3927 CPU/IO board */
#define MACH_TOSHIBA_RBTX4927	4
#define MACH_TOSHIBA_RBTX4937	5
#define GROUP_TOSHIBA_NAMES	{ "Pallas", "TopasCE", "JMR", "JMR TX3927", \
				  "RBTX4927", "RBTX4937" }

/*
 * Valid machtype for group LASAT
 */
#define MACH_LASAT_100		0	/* Masquerade II/SP100/SP50/SP25 */
#define MACH_LASAT_200		1	/* Masquerade PRO/SP200 */

/*
 * Valid machtypes for group Broadcom
 */
#define MACH_BCM93725		0
#define MACH_BCM93725_VJ	1
#define MACH_BCM93730		2
#define MACH_BCM947XX		3
#define MACH_BCM933XX		4
#define MACH_BCM96345		5
#define MACH_BCM96348		6

/*
 * Valid machtype for group Alchemy
 */
#define MACH_PB1000		0	/* Au1000-based eval board */
#define MACH_PB1100		1	/* Au1100-based eval board */
#define MACH_PB1500		2	/* Au1500-based eval board */
#define MACH_DB1000		3       /* Au1000-based eval board */
#define MACH_DB1100		4       /* Au1100-based eval board */
#define MACH_DB1500		5       /* Au1500-based eval board */
#define MACH_XXS1500		6       /* Au1500-based eval board */
#define MACH_MTX1		7	/* 4G MTX-1 Au1500-based board */

/*
 * Valid machtype for group NEC_VR41XX
 */
#define MACH_NEC_OSPREY		0	/* Osprey eval board */
#define MACH_NEC_EAGLE		1	/* NEC Eagle/Hawk board */
#define MACH_ZAO_CAPCELLA	2	/* ZAO Networks Capcella */
#define MACH_VICTOR_MPC30X	3	/* Victor MP-C303/304 */
#define MACH_IBM_WORKPAD	4	/* IBM WorkPad z50 */
#define MACH_CASIO_E55		5	/* CASIO CASSIOPEIA E-10/15/55/65 */
#define MACH_TANBAC_TB0226	6	/* TANBAC TB0226 (MBASE) */
#define MACH_TANBAC_TB0229	7	/* TANBAC TB0229 (VR4131DIMM) */

/*
 * Valid machtype for group Analog
 */
#define MACH_ADI_GATEWAY 	0

/*
 * Valid machtype for group ADM_GW
 */
#define MACH_ADM_GW_5120	0

#define CL_SIZE			(256)

const char *get_system_type(void);

extern unsigned long mips_machtype;
extern unsigned long mips_machgroup;

#define BOOT_MEM_MAP_MAX	32
#define BOOT_MEM_RAM		1
#define BOOT_MEM_ROM_DATA	2
#define BOOT_MEM_RESERVED	3

/*
 * A memory map that's built upon what was determined
 * or specified on the command line.
 */
struct boot_mem_map {
	int nr_map;
	struct {
		phys_t addr;	/* start of memory segment */
		phys_t size;	/* size of memory segment */
		long type;		/* type of memory segment */
	} map[BOOT_MEM_MAP_MAX];
};

extern struct boot_mem_map boot_mem_map;

extern void add_memory_region(phys_t start, phys_t size, long type);

#endif /* _ASM_BOOTINFO_H */
