/*
 * Cryptographic API.
 *
 * Support for Infineon DEU hardware crypto engine.
 *
 * Copyright (c) 2005  Johannes Doering <info@com-style.de>, INFINEON
 *
 * weak key check routine taken from crypto/des.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2002, Dr Brian Gladman <brg@gladman.me.uk>, Worcester, UK.
 * All rights reserved.
 *
 * LICENSE TERMS
 *
 * The free distribution and use of this software in both source and binary
 * form is allowed (with or without changes) provided that:
 *
 *   1. distributions of this source code include the above copyright
 *      notice, this list of conditions and the following disclaimer;
 *
 *   2. distributions in binary form include the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other associated materials;
 *
 *   3. the copyright holder's name is not used to endorse products
 *      built using this software without specific written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this product
 * may be distributed under the terms of the GNU General Public License (GPL),
 * in which case the provisions of the GPL apply INSTEAD OF those given above.
 *
 * DISCLAIMER
 *
 * This software is provided 'as is' with no explicit or implied warranties
 * in respect of its properties, including, but not limited to, correctness
 * and/or fitness for purpose.
 * ---------------------------------------------------------------------------
 * Change Log:
 * yclee 15 Jun 2006: tidy code; add local_irq_save() & local_irq_restore()
 * ---------------------------------------------------------------------------
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <asm/scatterlist.h>
#include <linux/crypto.h>
#include <linux/delay.h>

#define CONFIG_DANUBE_14        1   //  for Danube 1.4 chip

#ifdef CONFIG_CRYPTO_DEV_INCAIP1_DES
#include <asm/incaip/inca-ip.h>
#include <asm/incaip/incaip-deu-structs.h>
#define DES_3DES_START INCA_IP_DES_3DES
#endif
#ifdef CONFIG_CRYPTO_DEV_INCAIP2_DES
#include <asm/incaip2/incaip2.h>
#include <asm/incaip2/incaip2-deu.h>
#include <asm/incaip2/incaip2-deu-structs.h>
#define DES_3DES_START DES_CON
#endif
#ifdef CONFIG_CRYPTO_DEV_DANUBE_DES
#include <asm/danube/danube.h>
#include <asm/danube/danube_deu.h>
#include <asm/danube/danube_deu_structs.h>
#define DES_3DES_START	DANUBE_DES_CON
//#define ENDIAN_SWAP(x)  cpu_to_le32(x) //Westell
/*
  #ifdef CONFIG_DANUBE_14
    #define ENDIAN_SWAP(x)  cpu_to_le32(x)
  #else
    #define ENDIAN_SWAP(x)  (x)
  #endif
  */
#endif
#ifdef CONFIG_CRYPTO_DEV_DANUBE_DMA
#include "ifxdeu-dma.h"
#include <asm/danube/danube_dma.h>
#include <asm/danube/irq.h>
#endif

#ifdef CONFIG_CRYPTO_DEV_DANUBE_DMA

u8 *des_deu_dma_buffer_alloc(int len, int *byte_offset, void **opt)
{
	u8	*buffer = NULL;

	buffer = (u8 *) KSEG1ADDR(__get_free_page(GFP_DMA));

	//printk("buffer = %08X\n", (u32)buffer);

	//memset(buffer, 0x55, len);
	*byte_offset = 0;
	return buffer;
}
#endif  // CONFIG_CRYPTO_DEV_DANUBE_DMA

#define DES_KEY_SIZE		8
#define DES_EXPKEY_WORDS	32
#define DES_BLOCK_SIZE		8

#define DES3_EDE_KEY_SIZE		(3 * DES_KEY_SIZE)
#define DES3_EDE_EXPKEY_WORDS	(3 * DES_EXPKEY_WORDS)
#define DES3_EDE_BLOCK_SIZE		DES_BLOCK_SIZE

#ifdef CRYPTO_DEBUG
extern void hexdump(unsigned char *buf, unsigned int len); /* in ifxedu-generic.c*/
#endif

#define ROR(d, c, o)			((d) = (d) >> (c) | (d) << (o))

extern int	disable_multiblock;


//Westell
extern int chip_id;
#define ENDIAN_SWAP(x)  (chip_id  >= TWINPASS_REV14 ? cpu_to_le32(x) : x) //Westell
//end westell

struct des_ctx
{
	u8	iv[DES_BLOCK_SIZE];
	u32 expkey[DES_EXPKEY_WORDS];
};

struct des3_ede_ctx
{
	u8	iv[DES_BLOCK_SIZE];
	u32 expkey[DES3_EDE_EXPKEY_WORDS];
};

static const u8 parity[] = {
	8,1,0,8,0,8,8,0,0,8,8,0,8,0,2,8,0,8,8,0,8,0,0,8,8,0,0,8,0,8,8,3,
	0,8,8,0,8,0,0,8,8,0,0,8,0,8,8,0,8,0,0,8,0,8,8,0,0,8,8,0,8,0,0,8,
	0,8,8,0,8,0,0,8,8,0,0,8,0,8,8,0,8,0,0,8,0,8,8,0,0,8,8,0,8,0,0,8,
	8,0,0,8,0,8,8,0,0,8,8,0,8,0,0,8,0,8,8,0,8,0,0,8,8,0,0,8,0,8,8,0,
	0,8,8,0,8,0,0,8,8,0,0,8,0,8,8,0,8,0,0,8,0,8,8,0,0,8,8,0,8,0,0,8,
	8,0,0,8,0,8,8,0,0,8,8,0,8,0,0,8,0,8,8,0,8,0,0,8,8,0,0,8,0,8,8,0,
	8,0,0,8,0,8,8,0,0,8,8,0,8,0,0,8,0,8,8,0,8,0,0,8,8,0,0,8,0,8,8,0,
	4,8,8,0,8,0,0,8,8,0,0,8,0,8,8,0,8,5,0,8,0,8,8,0,0,8,8,0,8,0,6,8,
};



void deu_des_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	//printk("DEU Interrupt: %d\n", irq);
	//wait for processing
}


void initdes(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	volatile struct des_t	*des = (struct des_t *) DES_3DES_START;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(CONFIG_CRYPTO_DEV_INCAIP2) | defined(CONFIG_CRYPTO_DEV_DANUBE)
	des->controlr.ARS = 1;	// autostart
#endif

#ifdef CONFIG_CRYPTO_DEV_DANUBE_DMA
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct dma_device_info		*dma_device;
	volatile struct deu_dma_t	*dma = (struct deu_dma_t *) DMA_CON;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dma_device = dma_device_reserve("DEU");
	if(!dma_device) return -1;

	ifx_deu[0].dma_device = dma_device;

	dma->controlr.ALGO = 0; //DES
	dma->controlr.BS = 0;
	dma->controlr.EN = 1;

	// interrupt, but we don't need it
	//request_irq(DEU_DESIR,deu_des_interrupt,SA_INTERRUPT, "ifxdeu-aes",NULL);
	//*DANUBE_DEU_IRNEN = 7;   //enable all interrupts for testing
#endif
}

/*
 * RFC2451: Weak key checks SHOULD be performed.
 */
static int setkey(u32 *expkey, const u8 *key, unsigned int keylen, u32 *flags)
{
	u32						n, w;
	volatile struct des_t	*des = (struct des_t *) DES_3DES_START;
	unsigned long			flag;

	local_irq_save(flag);

	des->controlr.M = 0;			// des
	des->controlr.SM = 1;			// autostart
	initdes();

	local_irq_restore(flag);

#ifdef CRYPTO_DEBUG
	printk("Setting key to registers: key ");
	hexdump((unsigned char *)key, keylen);
#endif
	if(keylen != 8) return -EINVAL; /* unsupported key length */

	//Parity Checks

	n  = parity[key[0]]; n <<= 4;
	n |= parity[key[1]]; n <<= 4;
	n |= parity[key[2]]; n <<= 4;
	n |= parity[key[3]]; n <<= 4;
	n |= parity[key[4]]; n <<= 4;
	n |= parity[key[5]]; n <<= 4;
	n |= parity[key[6]]; n <<= 4;
	n |= parity[key[7]];
	w = 0x88888888L;

	if ((*flags & CRYPTO_TFM_REQ_WEAK_KEY)
	    && !((n - (w >> 3)) & w)) {  /* 1 in 10^10 keys passes this test */
		if (n < 0x41415151) {
			if (n < 0x31312121) {
				if (n < 0x14141515) {
					/* 01 01 01 01 01 01 01 01 */
					if (n == 0x11111111) goto weak;
					/* 01 1F 01 1F 01 0E 01 0E */
					if (n == 0x13131212) goto weak;
				} else {
					/* 01 E0 01 E0 01 F1 01 F1 */
					if (n == 0x14141515) goto weak;
					/* 01 FE 01 FE 01 FE 01 FE */
					if (n == 0x16161616) goto weak;
				}
			} else {
				if (n < 0x34342525) {
					/* 1F 01 1F 01 0E 01 0E 01 */
					if (n == 0x31312121) goto weak;
					/* 1F 1F 1F 1F 0E 0E 0E 0E (?) */
					if (n == 0x33332222) goto weak;
				} else {
					/* 1F E0 1F E0 0E F1 0E F1 */
					if (n == 0x34342525) goto weak;
					/* 1F FE 1F FE 0E FE 0E FE */
					if (n == 0x36362626) goto weak;
				}
			}
		} else {
			if (n < 0x61616161) {
				if (n < 0x44445555) {
					/* E0 01 E0 01 F1 01 F1 01 */
					if (n == 0x41415151) goto weak;
					/* E0 1F E0 1F F1 0E F1 0E */
					if (n == 0x43435252) goto weak;
				} else {
					/* E0 E0 E0 E0 F1 F1 F1 F1 (?) */
					if (n == 0x44445555) goto weak;
					/* E0 FE E0 FE F1 FE F1 FE */
					if (n == 0x46465656) goto weak;
				}
			} else {
				if (n < 0x64646565) {
					/* FE 01 FE 01 FE 01 FE 01 */
					if (n == 0x61616161) goto weak;
					/* FE 1F FE 1F FE 0E FE 0E */
					if (n == 0x63636262) goto weak;
				} else {
					/* FE E0 FE E0 FE F1 FE F1 */
					if (n == 0x64646565) goto weak;
					/* FE FE FE FE FE FE FE FE */
					if (n == 0x66666666) goto weak;
				}
			}
		}

		goto not_weak;
weak:
		*flags |= CRYPTO_TFM_RES_WEAK_KEY;
		return -EINVAL;
	}

not_weak:
	local_irq_save(flag);

	des->K1HR = *((u32 *) key + 0);
	des->K1LR = *((u32 *) key + 1);

	local_irq_restore(flag);

	return 0;
}


int des_setkey(void *ctx, const u8 *key, unsigned int keylen, u32 *flags)
{
	return setkey(ctx, key, keylen, flags);
}


static void des_incaip1(void *ctx_arg, uint8_t *out_arg, const uint8_t *in_arg,
			uint8_t *iv_arg, size_t nbytes, int encdec,
			int mode)
{
	volatile struct des_t	*des = (struct des_t *) DES_3DES_START;
	int						i = 0;
	int						nblocks = 0;
	unsigned long			flag;

	local_irq_save(flag);

	//hexdump(in_arg, nbytes);

#ifdef CRYPTO_DEBUG
	printk("hardware is running\n");
	printk("inarg: ");
	hexdump((unsigned char *)in_arg, nbytes);
	printk("encdec %x\n", encdec);
	printk("mode %x\n", mode);
	printk("nbytes %u\n", nbytes);
	printk("iv:\n");
	hexdump((unsigned char *)iv_arg, 8);
#endif
	/* E_D == 0 for encryption, E_D ==1 for secryption */
	des->controlr.E_D = !encdec;				//encryption
	des->controlr.O = mode - 1;					//0 ECB 1 CBC 2 OFB 3 CFB 4 CTR	hexdump(prin,sizeof(*des));
	if(mode > 1)
	{
		des->IVHR = (*(u32 *) iv_arg);
		des->IVLR = (*((u32 *) iv_arg + 1));
	};

	nblocks = nbytes / 4;

#ifndef CONFIG_CRYPTO_DEV_DANUBE_DMA
	for(i = 0; i < nblocks; i += 2)
	{
		/* wait for busy bit to clear */

		/*--- Workaround ----------------------------------------------------
		do a dummy read to the busy flag because it is not raised early
		enough in CFB/OFB 3DES modes */
#ifdef CRYPTO_DEBUG
		printk("ihr: %x\n", (*((u32 *) in_arg + i)));
		printk("ilr: %x\n", (*((u32 *) in_arg + 1 + i)));
#endif
		des->IHR =  ENDIAN_SWAP(*((u32 *) in_arg + i));
		des->ILR =  ENDIAN_SWAP(*((u32 *) in_arg + 1 + i)); /* start crypto */

		while(des->controlr.BUS)
		{
			// this will not take long
		}

		*((u32 *) out_arg + 0 + i) = des->OHR;
		*((u32 *) out_arg + 1 + i) = des->OLR;
	}

#else
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct dma_device_info		*dma_device = ifx_deu[0].dma_device;
	volatile struct deu_dma_t	*dma = (struct deu_dma_t *) DMA_CON;
	u32							*out_dma = NULL;
	_ifx_deu_device				*pDev = ifx_deu;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDev->len = nbytes;
	pDev->packet_size = nbytes;
	pDev->src = in_arg;

	pDev->dst = out_arg;
	pDev->dst_count = 0;

	dma_device->num_rx_chan = 1;
	dma_device->num_tx_chan = 1;

	for(i = 0; i < dma_device->num_rx_chan; i++)
	{
		dma_device->rx_chan[i]->packet_size = pDev->packet_size;
		dma_device->rx_chan[i]->desc_len = 1;
		dma_device->rx_chan[i]->disable_irq(dma_device->rx_chan[i]);
		dma_device->rx_chan[i]->control = DANUBE_DMA_CH_ON;
		dma_device->rx_chan[i]->byte_offset = 0;
	}

	for(i = 0; i < dma_device->num_tx_chan; i++)
	{
		dma_device->tx_chan[i]->control = DANUBE_DMA_CH_ON;
	}

	dma_device->buffer_alloc = &des_deu_dma_buffer_alloc;
	dma_device->buffer_free = &deu_dma_buffer_free;

	//dma_device->intr_handler=&deu_dma_intr_handler;
	dma_device_register(dma_device);

	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int x = 0;
	u32 *out_dmaarr = out_arg;
	u32 *in_dmaarr = in_arg;
	u32 *dword_aligned_mem =  NULL;
	int rlen = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < dma_device->max_rx_chan_num; i++)
	{
		if((dma_device->rx_chan[i])->control == DANUBE_DMA_CH_ON)
			(dma_device->rx_chan[i])->open(dma_device->rx_chan[i]);
	}

	u8	*incopy = kmalloc(nbytes, GFP_KERNEL);
	if(incopy == NULL)
		return;
	else
	{
		dword_aligned_mem = (u32 *) incopy;					// need to do u32-based copy
		memcpy(incopy, in_arg, nbytes);
	}

	while(dma->controlr.BSY)
	{
		// this will not take long
	}

	while(des->controlr.BUS);
	{
		// this will not take long
	}

	volatile int hello = 500;
	while(hello--)
	{
	udelay(hello);
	}

	//	rlen=	dma_device_write(dma_device,(u8 *)in_arg,nbytes,in_arg);
	rlen = dma_device_write(dma_device, (u8 *) incopy, nbytes, incopy);

	udelay(10);

	while(dma->controlr.BSY)
	{
		// this will not take long
	}

	while(des->controlr.BUS);
	{
		// this will not take long
	}

	pDev->recv_count = dma_device_read(dma_device, &out_dma, NULL);
	udelay(10);

	while(dma->controlr.BSY)
	{
		// this will not take long
	}

	for(i = 0; i < (nbytes / 4); i++)
	{
		x = i ^ 1;
		dword_aligned_mem[i] = out_dma[x];
	}

	memcpy((u8 *) out_dmaarr, incopy, nbytes);	// note that incopy points to dword_aligned_mem
	if(incopy)
	{
		kfree(incopy);
	}

	if(out_dma)
	{
		//printk("out_dma = %08X\n", out_dma);
		free_page((u32) out_dma);
		out_dma = NULL;
	}

	des->controlr.SM = 0;						//needed for other modules

	//on the end after read unregister again
	//dma_device=pDev->dma_device;
	if(dma_device) dma_device_release(dma_device);

	if(pDev->recv_count == pDev->len)
	{
		dma_device_unregister(pDev->dma_device);
		dma_device_release(pDev->dma_device);
	}

#endif
#ifdef CRYPTO_DEBUG
	printk("outarg: ");
	hexdump(out_arg, nbytes);

	//printk("ohr : %x \n",des->OHR);
	//printk("ohl : %x \n",des->OLR);
#endif

	local_irq_restore(flag);
}

static void
des_incaip1_ecb(void *ctx, uint8_t *dst, const uint8_t *src,
		uint8_t *iv, size_t nbytes, int encdec, int inplace)
{
	//printk("des_incaip1_ecb\n");
	des_incaip1(ctx, dst, src, NULL, nbytes, encdec, CRYPTO_TFM_MODE_ECB);
}

void
des_incaip1_cbc(void *ctx, uint8_t *dst, const uint8_t *src, uint8_t *iv,
		size_t nbytes, int encdec, int inplace)
{
	//printk("des_incaip1_cbc\n");
	des_incaip1(ctx, dst, src, iv, nbytes, encdec, CRYPTO_TFM_MODE_CBC);
}

static void
des_incaip1_cfb(void *ctx, uint8_t *dst, const uint8_t *src, uint8_t *iv,
		size_t nbytes, int encdec, int inplace)
{
	//printk("des_incaip1_cfb\n");
	des_incaip1(ctx, dst, src, iv, nbytes, encdec, CRYPTO_TFM_MODE_CFB);
}

static void
des_incaip1_ofb(void *ctx, uint8_t *dst, const uint8_t *src, uint8_t *iv,
		size_t nbytes, int encdec, int inplace)
{
	//printk("des_incaip1_ofb\n");
	des_incaip1(ctx, dst, src, iv, nbytes, encdec, CRYPTO_TFM_MODE_OFB);
}


static void des_encrypt(void *ctx_arg, uint8_t *out, const uint8_t *in)
{
	//printk("des_encrypt\n");
	des_incaip1(ctx_arg, out, in, NULL, DES_BLOCK_SIZE, CRYPTO_DIR_ENCRYPT, CRYPTO_TFM_MODE_ECB);
}


static void des_decrypt(void *ctx_arg, uint8_t *out, const uint8_t *in)
{
	//printk("des_decrypt\n");
	des_incaip1(ctx_arg, out, in, NULL, DES_BLOCK_SIZE, CRYPTO_DIR_DECRYPT, CRYPTO_TFM_MODE_ECB);
}

/*
 * RFC2451:
 *
 *   For DES-EDE3, there is no known need to reject weak or
 *   complementation keys.  Any weakness is obviated by the use of
 *   multiple keys.
 *
 *   However, if the first two or last two independent 64-bit keys are
 *   equal (k1 == k2 or k2 == k3), then the DES3 operation is simply the
 *   same as DES.  Implementers MUST reject keys that exhibit this
 *   property.
 *
 */
int des3_ede_setkey(void *ctx, const u8 *key, unsigned int keylen, u32 *flags)
{
	volatile struct des_t	*des = (struct des_t *) DES_3DES_START;
	unsigned long			flag;

	local_irq_save(flag);

	/* Hardware Section */
	switch(keylen)
	{
    	case 24:	des->K3HR = *((u32 *) key + 4); des->K3LR = *((u32 *) key + 5);
    	/* no break; */

    	case 16:	des->K2HR = *((u32 *) key + 2); des->K2LR = *((u32 *) key + 3);

    	/* no break; */
    	case 8:		des->K1HR = *((u32 *) key + 0); des->K1LR = *((u32 *) key + 1);
    		break;

    	default:	return -EINVAL;
	}

	des->controlr.M = keylen / 8 + 1;	// 3DES EDE1 / EDE2 / EDE3 Mode
	des->controlr.SM = 1;				// start crypto engine with write to ILR
	initdes();

	local_irq_restore(flag);

	return 0;
}

static struct crypto_alg	des_alg =
{
	.cra_name = "des",
	.cra_preference = CRYPTO_PREF_HARDWARE,
	.cra_flags = CRYPTO_ALG_TYPE_CIPHER,
	.cra_blocksize = DES_BLOCK_SIZE,
	.cra_ctxsize = sizeof(struct des_ctx),
	.cra_module = THIS_MODULE,
	.cra_list =
	LIST_HEAD_INIT(des_alg.cra_list),
	.cra_u =
	{
		.cipher = {
					.cia_min_keysize	=	DES_KEY_SIZE,
					.cia_max_keysize	=	DES_KEY_SIZE,
					.cia_setkey		= 	des_setkey,
					.cia_encrypt		=	des_encrypt,
					.cia_decrypt		=	des_decrypt
		}
	}
};

static struct crypto_alg	des3_ede_alg =
{
	.cra_name = "des3_ede",
	.cra_preference = CRYPTO_PREF_HARDWARE,
	.cra_flags = CRYPTO_ALG_TYPE_CIPHER,
	.cra_blocksize = DES3_EDE_BLOCK_SIZE,
	.cra_ctxsize = sizeof(struct des3_ede_ctx),
	.cra_module = THIS_MODULE,
	.cra_list =
	LIST_HEAD_INIT(des3_ede_alg.cra_list),
	.cra_u =
	{
		.cipher = {
        	.cia_min_keysize	=	DES3_EDE_KEY_SIZE,
        	.cia_max_keysize	=	DES3_EDE_KEY_SIZE,
        	.cia_setkey	   	= 	des3_ede_setkey,
        	.cia_encrypt	 	=	des_encrypt,
        	.cia_decrypt	  	=	des_decrypt
        }
	}
};



int __init ifxdeu_init_des(void)
{
	int ret = 0;

	printk(KERN_NOTICE "Using Infineon DEU for DES algorithm%s.\n", disable_multiblock ? "" : " (multiblock)");

	return ret;
}


void __exit ifxdeu_fini_des(void)
{
}
