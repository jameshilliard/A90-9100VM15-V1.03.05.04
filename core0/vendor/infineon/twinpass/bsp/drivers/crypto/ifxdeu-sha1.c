/*
 *  Cryptographic API.
 *
 * Support for Infineon DEU hardware crypto engine.
 *
 * Copyright (c) 2005  Johannes Doering <info@com-style.de>, INFINEON
 *
 * SHA1 Secure Hash Algorithm.
 *
 * Derived from cryptoapi implementation, adapted for in-place
 * scatterlist interface.  Originally based on the public domain
 * implementation written by Steve Reid.
 *
 * Copyright (c) Alan Smithee.
 * Copyright (c) Andrew McDonald <andrew@mcdonald.org.uk>
 * Copyright (c) Jean-Francois Dive <jef@linuxbe.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * ---------------------------------------------------------------------------
 * Change Log:
 * yclee 15 Jun 2006: tidy code; add local_irq_save() & local_irq_restore()
 * ---------------------------------------------------------------------------
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/crypto.h>
#include <asm/scatterlist.h>
#include <asm/byteorder.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/delay.h>

#ifdef CONFIG_CRYPTO_DEV_INCAIP1_SHA1
#include <asm/incaip/inca-ip.h>
#include <asm/incaip/inca-ip-deu-structs.h>
#define HASH_START	INCA_IP_AES_AES
#endif
#ifdef CONFIG_CRYPTO_DEV_INCAIP2_SHA1
#include <asm/incaip2/incaip2_dma.h>
#include <asm/incaip2/incaip2-deu.h>
#include <asm/incaip2/incaip2-deu-structs.h>

// address of hash_con starting
#define HASH_START	HASH_CON
#endif
#ifdef CONFIG_CRYPTO_DEV_DANUBE_SHA1
#include <asm/danube/danube.h>
#include <asm/danube/danube_deu.h>
#include <asm/danube/danube_deu_structs.h>

// address of hash_con starting
#define HASH_START	DANUBE_HASH_CON
#endif
#ifdef CONFIG_CRYPTO_DEV_DANUBE_DMA
#include <asm/danube/danube_dma.h>
#include "ifxdeu-dma.h"
#endif
#undef DEBUG_SHA1

//#define ONLY_IN_MEM
#define SHA1_DIGEST_SIZE		20
#define SHA1_HMAC_BLOCK_SIZE	64

extern int	disable_multiblock;
extern int	disable_deudma;



static inline u32 rol(u32 value, u32 bits)
{
	return(((value) << (bits)) | ((value) >> (32 - (bits))));
}

/* blk0() and blk() perform the initial expand. */

/* I got the idea of expanding during the round function from SSLeay */
#define blk0(i) block32[i]

#define blk(i) (block32[i&15] = rol(block32[(i+13)&15]^block32[(i+8)&15] \
    ^block32[(i+2)&15]^block32[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5); \
                        w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5); \
                        w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5); \
                        w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);



/* Parameters for deu DEU device */
#define DEFAULT_DEU_CHANNEL_WEIGHT			1;
#define DEFAULT_DEU_PORT_WEIGHT				1;

#define DEFAULT_DEU_TX_BURST_LEN			2
#define DEFAULT_DEU_RX_BURST_LEN			2

#define DEFAULT_DEU_TX_CHANNEL_NUM			1
#define DEFAULT_DEU_RX_CHANNEL_NUM			1

#define DEFAULT_DEU_TX_CHANNEL_DESCR_NUM	20
#define DEFAULT_DEU_RX_CHANNEL_DESCR_NUM	20

u32 deu_g_dma_rx = 0, deu_g_dma_alloc = 0, deu_g_dma_rx_err = 0, deu_g_dma_alloc_drop = 0;
u32 deu_g_dma_free = 0, deu_g_dma_tx = 0, deu_g_dma_tx_full = 0, deu_g_dma_tx_drop = 0, deu_g_dma_free_err = 0;

struct sha1_hw_ctx
{
	struct dma_device_info	*dma_device;
	u64						count;
	u32						state[5];
	u8						buffer[64];
};

/* Hash a single 512-bit block. This is the core of the algorithm. */
static void sha1_transform(u32 *state, const u32 *in)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i = 0;
	unsigned long			    flag;
#ifndef ONLY_IN_MEM
	volatile struct deu_hash_t	*hashs = (struct deu_hash_t *) HASH_START;
#ifdef CONFIG_CRYPTO_DEV_DANUBE_DMA
	volatile struct deu_dma_t	*dma = (struct deu_dma_t *) DMA_CON;
#endif	
#else
	struct deu_hash_t			*hashs = NULL;
	unsigned char				*prin = NULL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	hashs = (struct deu_hash_t *) kmalloc(sizeof(*hashs), GFP_KERNEL);
	memset(hashs, 0, sizeof(*hashs));
	prin = (unsigned char *) hashs;
#endif

	local_irq_save(flag);

#ifdef DEBUG_SHA1
	printk("sha1_transform running\n");
#endif //debug SHA1
#ifndef CONFIG_CRYPTO_DEV_DANUBE_DMA	//dma or not
	asm("sync");
	for(i = 0; i < 16; i++)
	{
#ifdef DEBUG_SHA1
		printk("%u:%x\n", i, in[i]);
#endif //debug SHA1
		hashs->MR = in[i];
		asm("sync");
	};

	//wait for processing
	while(hashs->controlr.BSY)
	{
		// this will not take long
	}

#else \
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/ \
	//dma
	struct dma_device_info	*dma_device = ifx_deu[0].dma_device;
	_ifx_deu_device			*pDev = ifx_deu;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pDev->len = sizeof(in);
	pDev->src = in;

	while(dma->controlr.BSY)
	{
		// this will not take long
	}

	dma_device_write(dma_device, (u8 *) in, 64, NULL);
	udelay(10);

	while(dma->controlr.BSY)
	{
		// this will not take long
	}
#endif

	local_irq_restore(flag);
}


static void sha1_init(void *ctx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct sha1_hw_ctx			*sctx = ctx;
	unsigned long			    flag;
#ifndef ONLY_IN_MEM
	volatile struct deu_hash_t	*hash = (struct deu_hash_t *) HASH_START;
#else
	struct deu_hash_t			*hash = NULL;
	unsigned char				*prin = NULL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	hash = (struct deu_hash_t *) kmalloc(sizeof(*hash), GFP_KERNEL);
	memset(hash, 0, sizeof(*hash));
	prin = (unsigned char *) hash;
#endif

	local_irq_save(flag);

#ifdef DEBUG_SHA1
	printk("sha1_init running\n");
	hexdump(hash, sizeof(*hash));
#endif //debug SHA1

	//	hash->controlr.DAU=0
	hash->controlr.SM = 1;
	hash->controlr.ALGO = 0;	// 1 = md5  0 = sha1
	hash->controlr.INIT = 1;	// Initialize the hash operation by writing a '1' to the INIT bit.

	memset(sctx, 0, sizeof(struct sha1_hw_ctx));

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef CONFIG_CRYPTO_DEV_DANUBE_DMA
	struct dma_device_info		*dma_device;
	volatile struct deu_dma_t	*dma = (struct deu_dma_t *) DMA_CON;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dma_device = dma_device_reserve("DEU");

	/*~~~~~~*/
	int i = 0;
	/*~~~~~~*/

	if(!dma_device) return;		//-1;
	ifx_deu[0].dma_device = dma_device;

	dma->controlr.ALGO = 2;		//10

	//dma->controlr.BS = 1;
	dma->controlr.EN = 1;

	dma_device->num_rx_chan = 0;
	dma_device->num_tx_chan = 1;

	for(i = 0; i < dma_device->num_tx_chan; i++)
	{
		//dma_device->rx_chan[i]->packet_size=pDev->packet_size;
		dma_device->tx_chan[i]->control = DANUBE_DMA_CH_ON;
	}

	//--------------------------------------------------------------------------------
	// register the call-back functions provided by dma driver
	//--------------------------------------------------------------------------------
	dma_device->buffer_alloc = &deu_dma_buffer_alloc;
	dma_device->buffer_free = &deu_dma_buffer_free;
	dma_device->intr_handler = &deu_dma_intr_handler;
	dma_device_register(dma_device);
#endif // CONFIG_CRYPTO_DEV_DANUBE_DMA
#ifdef DEBUG_SHA1
	hexdump(hash, sizeof(*hash));
#endif //debug SHA1

	local_irq_restore(flag);

}


static void sha1_update(void *ctx, const u8 *data, unsigned int len)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct sha1_hw_ctx	*sctx = ctx;
	unsigned int		i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	j = (sctx->count >> 3) & 0x3f;
	sctx->count += len << 3;

	if((j + len) > 63)
	{
		memcpy(&sctx->buffer[j], data, (i = 64 - j));
		sha1_transform(sctx->state, (const u32 *)sctx->buffer);
		for(; i + 63 < len; i += 64)
		{
			sha1_transform(sctx->state, (const u32 *)&data[i]);
		}

		j = 0;
	}
	else
		i = 0;

	memcpy(&sctx->buffer[j], &data[i], len - i);
}

/* Add padding and return the message digest. */
static void sha1_final(void *ctx, u8 *out)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	struct sha1_hw_ctx			*sctx = ctx;
	u32 index, padlen;
	u64							t;
	u8							bits[8] = { 0, };
	static const u8				padding[64] = { 0x80, };
	unsigned long			    flag;
#ifndef ONLY_IN_MEM
	volatile struct deu_hash_t	*hashs = (struct deu_hash_t *) HASH_START;
#else
	struct deu_hash_t			*hashs = NULL;
	unsigned char				*prin = NULL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	hashs = (struct deu_hash_t *) kmalloc(sizeof(*hashs), GFP_KERNEL);
	memset(hashs, 0, sizeof(*hashs));
	prin = (unsigned char *) hashs;
#endif
	t = sctx->count;
	bits[7] = 0xff & t; t>>=8;
	bits[6] = 0xff & t; t>>=8;
	bits[5] = 0xff & t; t>>=8;
	bits[4] = 0xff & t; t>>=8;
	bits[3] = 0xff & t; t>>=8;
	bits[2] = 0xff & t; t>>=8;
	bits[1] = 0xff & t; t>>=8;
	bits[0] = 0xff & t;

	/* Pad out to 56 mod 64 */
	index = (sctx->count >> 3) & 0x3f;
	padlen = (index < 56) ? (56 - index) : ((64 + 56) - index);
	sha1_update(sctx, padding, padlen);

	/* Append length */
	sha1_update(sctx, bits, sizeof bits);

	local_irq_save(flag);

#ifdef CONFIG_CRYPTO_DEV_DANUBE_DMA
	//wait for processing
	while(hashs->controlr.BSY)
	{
		// this will not take long
	}
#endif
	*((u32 *) out + 0) = hashs->D1R;
	*((u32 *) out + 1) = hashs->D2R;
	*((u32 *) out + 2) = hashs->D3R;
	*((u32 *) out + 3) = hashs->D4R;
	*((u32 *) out + 4) = hashs->D5R;

	/*
	// Store state in digest
	for (i = j = 0; i < 5; i++, j += 4) {
		u32 t2 = sctx->state[i];
		out[j+3] = t2 & 0xff; t2>>=8;
		out[j+2] = t2 & 0xff; t2>>=8;
		out[j+1] = t2 & 0xff; t2>>=8;
		out[j  ] = t2 & 0xff;
	}

	*/
	hashs->controlr.SM = 0; // switch off again for next dma transfer

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef CONFIG_CRYPTO_DEV_DANUBE_DMA
	struct dma_device_info	*dma_device;
	_ifx_deu_device			*pDev = ifx_deu;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dma_device = pDev->dma_device;

	//if(dma_device) dma_device_release(dma_device);
	if(dma_device)
	{
		dma_device_unregister(pDev->dma_device);
		dma_device_release(pDev->dma_device);
	}
#endif

	local_irq_restore(flag);

	// Wipe context
	memset(sctx, 0, sizeof *sctx);
}

static struct crypto_alg	alg =
{
	.cra_name = "sha1",
	.cra_preference = CRYPTO_PREF_HARDWARE,
	.cra_flags = CRYPTO_ALG_TYPE_DIGEST,
	.cra_blocksize = SHA1_HMAC_BLOCK_SIZE,
	.cra_ctxsize = sizeof(struct sha1_hw_ctx),
	.cra_module = THIS_MODULE,
	.cra_list =
	LIST_HEAD_INIT(alg.cra_list),
	.cra_u =
	{
		.digest =
		{
			.dia_digestsize = SHA1_DIGEST_SIZE,
            .dia_init = sha1_init,
            .dia_update = sha1_update,
            .dia_final = sha1_final
		}
	}
};



int __init ifxdeu_init_sha1(void)
{
	printk
	(
		KERN_NOTICE "Using Infineon DEU for SHA1 algorithm%s. %s\n",
		disable_multiblock ? "" : " (multiblock)",
		disable_deudma ? "" : " (DMA)"
	);

	return 0;
}


void __exit ifxdeu_fini_sha1(void)
{
}
