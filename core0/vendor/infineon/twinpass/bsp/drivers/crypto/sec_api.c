/****************************************************************************
 *
 * rg/vendor/infineon/twinpass/bsp/drivers/crypto/sec_api.c
 * 
 * Copyright (C) Jungo LTD 2004
 * 
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General 
 * Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02111-1307, USA.
 *
 * Developed by Jungo LTD.
 * Residential Gateway Software Division
 * www.jungo.com
 * info@jungo.com
 */

#include <linux/types.h>   
#include <linux/kernel.h>
#include <linux/module.h> 
#include <string.h>

#define DES_KEY_SIZE		8
#define DES_EXPKEY_WORDS	32
#define DES_BLOCK_SIZE		8

#define DES3_EDE_KEY_SIZE	(3 * DES_KEY_SIZE)
#define DES3_EDE_EXPKEY_WORDS	(3 * DES_EXPKEY_WORDS)
#define DES3_EDE_BLOCK_SIZE	DES_BLOCK_SIZE

extern int des_setkey(void *ctx, const u8 *key, unsigned int keylen,
    u32 *flags);
extern void des_incaip1_cbc(void *ctx, uint8_t *dst, const uint8_t *src,
    uint8_t *iv, size_t nbytes, int encdec, int inplace);
extern int des3_ede_setkey(void *ctx, const u8 *key, unsigned int keylen,
    u32 *flags);

#define AES_MAX_KEY_SIZE		32	/* ditto */
#define AES_BLOCK_SIZE			16	/* ditto */
extern int aes_set_key(void *ctx_arg, const uint8_t *in_key,
    unsigned int key_len, uint32_t *flags);
extern void aes_ifxdeu_cbc(void *ctx, uint8_t *dst, const uint8_t *src,
    uint8_t *iv, size_t nbytes, int encdec, int inplace);

typedef struct {
    uint32_t   aes_key_len;  /* key_len */
    uint8_t    aes_key[AES_MAX_KEY_SIZE];  /* the encryption key schedule */
} aes_context;

#define MD5_BLOCK_WORDS			16 /* 128 bit digest */
#define MD5_HASH_WORDS			4 
#define MD5_HMAC_BLOCK_SIZE		64 /* 512 bit block */

extern void md5_init(void *ctx);
extern void md5_update(void *ctx, const u8 *data, unsigned int len);
extern void md5_final(void *ctx, u8 *out);

struct md5_ctx {
    u32 hash[MD5_HASH_WORDS];
    u32 block[MD5_BLOCK_WORDS];
    u64 byte_count;
};

#define SHA1_HMAC_BLOCK_SIZE		64 /* 512 bit block */
#define SHA1_DIGEST_SIZE		20 /* 160 bit digest */

extern void sha1_init(void *ctx);
extern void sha1_update(void *ctx, const u8 *data, unsigned int len);
extern void sha1_final(void *ctx, u8 *out);

struct sha1_hw_ctx {
    struct dma_device_info *dma_device;
    u64	count;
    u32	state[5];
    u8	buffer[64];
};

#ifdef TWINPASS_CRYPTO_DEBUG
#define DEBUG_PRINT(fmt, p...) \
    printk("Infineon crypto debug: %s: " fmt, __FUNCTION__, ##p)
    extern void hexdump(unsigned char *buf, unsigned int len);
#else
#define DEBUG_PRINT(fmt, p...)
#endif

int _des_twinpass_set_key(void *alg, __u8 *key_e, const __u8 *key, 
    size_t keysize)
{
    DEBUG_PRINT("alg=%p, key_e=%p, key=%p keysize=%d \n",
	alg, key_e, key, keysize);

    memcpy(key_e, key, keysize);
    return 0;
}

int _1des_twinpass_cbc_encrypt(void *alg, __u8 *key_e, __u8 *in, int ilen,
    const __u8 *iv, int encrypt)
{
    __u8 out[ilen];
    u32 flags = 0, ret;
    char iv_buf[DES_BLOCK_SIZE];

    ((__u32 *)iv_buf)[0] = ((__u32 *)(iv))[0];
    ((__u32 *)iv_buf)[1] = ((__u32 *)(iv))[1];
    
    DEBUG_PRINT("alg=%p, key_e=%p, in=%p, ilen=%d, iv=%p, encrypt=%d\n",
	alg, key_e, in, ilen, iv, encrypt);
    
    ret = des_setkey(NULL, key_e, DES_KEY_SIZE, &flags); 
    des_incaip1_cbc(alg, out, in, (uint8_t *)iv_buf, ilen, encrypt, 0);
    DEBUG_PRINT("set_key returned flags=%x ret=%d\n", flags,
	ret);

    memcpy(in, out, ilen);
    return ret ? -1 : ilen;
}

int _3des_twinpass_cbc_encrypt(void *alg, __u8 *key_e, __u8 *in, int ilen,
    const __u8 *iv, int encrypt)
{
    __u8 out[ilen];
    u32 flags = 0, ret;
    char iv_buf[DES3_EDE_BLOCK_SIZE];

    ((__u32 *)iv_buf)[0] = ((__u32 *)(iv))[0];
    ((__u32 *)iv_buf)[1] = ((__u32 *)(iv))[1];
    
    DEBUG_PRINT("alg=%p, key_e=%p, in=%p, ilen=%d, iv=%p, encrypt=%d\n",
	alg, key_e, in, ilen, iv, encrypt);
    
    ret = des3_ede_setkey(NULL, key_e, DES3_EDE_KEY_SIZE, &flags); 
    des_incaip1_cbc(alg, out, in, (uint8_t *)iv_buf, ilen, encrypt, 0);
    DEBUG_PRINT("set_key returned flags=%x ret=%d\n", flags,
	ret);

    memcpy(in, out, ilen);
    return ret? -1 : ilen;
}

int _aes_twinpass_set_key(void *alg, __u8 * key_e, const __u8 * key, 
    size_t keysize)
{
    aes_context *ctx = (aes_context *)key_e;

    if (keysize != 32 && keysize != 24 && keysize != 16)
	return -1;

    DEBUG_PRINT("alg=%p, key_e=%p, key=%p keysize=%d \n", alg, key_e, key,
	keysize);
    
    ctx->aes_key_len = keysize;
    memcpy(ctx->aes_key, key, keysize);

    return 0;
}

int _aes_twinpass_cbc_encrypt(void *alg, __u8 * key_e, __u8 * in, int ilen,
    const __u8 * iv, int encrypt)
{
    u32 ret, flags = 0, len, block_no;
    aes_context *ctx = (aes_context *)key_e;
    __u8 out[ilen], *p_out = out, *p_in = in;

    memset(out, 0, sizeof(out));
    if (ilen % AES_BLOCK_SIZE)
	printk("%s Error: length is not aligned to blocksize", __FUNCTION__);

    DEBUG_PRINT("alg=%p, key_e=%p, in=%p, ilen=%d, iv=%p, encrypt=%d\n"
	" key_e=%02x%02x%02x%02x, key_len=%d", alg, key_e, in, ilen, iv,
	encrypt, ctx->aes_key[0], ctx->aes_key[1], ctx->aes_key[2],
	ctx->aes_key[3], ctx->aes_key_len);
   
    ret = aes_set_key(NULL, ctx->aes_key, ctx->aes_key_len, &flags);
    DEBUG_PRINT("set_key returned flags=%x ret=%d\n", flags, ret);

    for (len = ilen, block_no = 0; len >= AES_BLOCK_SIZE;
	len -= AES_BLOCK_SIZE, block_no++)
    {
	aes_ifxdeu_cbc(alg, p_out, p_in, block_no ? NULL : (uint8_t *)iv,
	    AES_BLOCK_SIZE, encrypt, 0);
	p_in += AES_BLOCK_SIZE;
	p_out += AES_BLOCK_SIZE;
    }

    memcpy(in, out, ilen);
    return ret ? -1 : ilen;
}

/* MD5 */
typedef struct md5_hmac_context {
    u8 k_opad[MD5_HMAC_BLOCK_SIZE + 1]; /* Inner pad - key XORd with ipad */
    u8 k_ipad[MD5_HMAC_BLOCK_SIZE + 1]; /* Outer pad - key XORd with opad */
} md5_hmac_context;

int _md5_twinpass_hmac_set_key(void *alg, __u8 * key_a, const __u8 * key, 
    int keylen) 
{
    int i;
    u8 key_[MD5_HMAC_BLOCK_SIZE];
    md5_hmac_context *ctx = (md5_hmac_context *)key_a;

    DEBUG_PRINT("alg=%p, key_a=%p, key=%p, keylen=%d ", alg, key_a, key,
	keylen);

    /* If key is longer than 64 bytes digest it to key=MD5(key) */
    if (keylen > MD5_HMAC_BLOCK_SIZE) 
    {
	struct md5_ctx mctx; /* DEU context */

	md5_init(&mctx);
	md5_update(&mctx, key, keylen);
	md5_final(&mctx, key_);

	keylen = MD5_BLOCK_WORDS; /* digest size */
    }
    else
	memcpy(key_, key, keylen);

    /* Store key in pads */
    memset(ctx->k_ipad, 0, sizeof(ctx->k_ipad));
    memset(ctx->k_opad, 0, sizeof(ctx->k_opad));
    memcpy(ctx->k_ipad, key_, keylen);
    memcpy(ctx->k_opad, key_, keylen);

    /* XOR key with ipad and opad values */
    for (i=0; i<MD5_HMAC_BLOCK_SIZE; i++) 
    {
	ctx->k_ipad[i] ^= 0x36;
	ctx->k_opad[i] ^= 0x5c;
    }

    return 0;
}

/* The HMAC_MD5 transform looks like:
 * MD5(K XOR opad, MD5(K XOR ipad, text))
 * where 'K' is an n byte key, 'ipad' is the byte 0x36 repeated 64 times,
 * 'opad' is the byte 0x5c repeated 64 times and 'text' is the data being
 * hashed.
 */
int _md5_twinpass_hmac_hash(void *alg, __u8 * key_a, const __u8 * dat, int len,
    __u8 * hash, int hashlen) 
{
    struct md5_ctx mctx; /* DEU context */
    md5_hmac_context *ctx = (md5_hmac_context *)key_a; /* sec_api context */
    u8 digest[MD5_BLOCK_WORDS];

    /* MD5(K XOR ipad, text) */
    md5_init(&mctx);
    md5_update(&mctx, ctx->k_ipad, MD5_HMAC_BLOCK_SIZE);
    md5_update(&mctx, dat, len);
    md5_final(&mctx, digest);

    /* MD5(K XOR opad, digest) */
    md5_init(&mctx);
    md5_update(&mctx, ctx->k_opad, MD5_HMAC_BLOCK_SIZE);
    md5_update(&mctx, digest, sizeof(digest));
    md5_final(&mctx, hash);

   return 0;
}

/* SHA-1 */
typedef struct sha1_hmac_context {
    u8 k_opad[SHA1_HMAC_BLOCK_SIZE + 1]; /* Inner pad - key XORd with ipad */
    u8 k_ipad[SHA1_HMAC_BLOCK_SIZE + 1]; /* Outer pad - key XORd with opad */
} sha1_hmac_context;

int _sha1_twinpass_hmac_set_key(void *alg, __u8 * key_a, const __u8 * key, 
    int keylen) 
{
    int i;
    u8 key_[SHA1_HMAC_BLOCK_SIZE];
    sha1_hmac_context *ctx = (sha1_hmac_context *)key_a;

    DEBUG_PRINT("alg=%p, key_a=%p, key=%p, keylen=%d ", alg, key_a, key,
	keylen); 

    /* If key is longer than 64 bytes digest it to key = SHA1(key) */
    if (keylen > SHA1_HMAC_BLOCK_SIZE) 
    {
	struct sha1_hw_ctx mctx; /* DEU context */

	sha1_init(&mctx);
	sha1_update(&mctx, key, keylen);
	sha1_final(&mctx, key_);

	keylen = SHA1_DIGEST_SIZE; /* digest size */
    }
    else
	memcpy(key_, key, keylen);

    /* Store key in pads */
    memset(ctx->k_ipad, 0, sizeof(ctx->k_ipad));
    memset(ctx->k_opad, 0, sizeof(ctx->k_opad));
    memcpy(ctx->k_ipad, key_, keylen);
    memcpy(ctx->k_opad, key_, keylen);

    /* XOR key with ipad and opad values */
    for (i = 0; i < SHA1_HMAC_BLOCK_SIZE; i++) 
    {
	ctx->k_ipad[i] ^= 0x36;
	ctx->k_opad[i] ^= 0x5c;
    }

    return 0;
}

/* The HMAC_SHA1 transform looks like:
 * SHA1(K XOR opad, SHA1(K XOR ipad, text))
 * where 'K' is an n byte key, 'ipad' is the byte 0x36 repeated 64 times,
 * 'opad' is the byte 0x5c repeated 64 times and 'text' is the data being
 * hashed.
 */
int _sha1_twinpass_hmac_hash(void *alg, __u8 * key_a, const __u8 * dat, int len,
    __u8 * hash, int hashlen) 
{
    struct sha1_hw_ctx mctx; /* DEU context */
    sha1_hmac_context *ctx = (sha1_hmac_context *)key_a; /* sec_api context */
    u8 digest[SHA1_DIGEST_SIZE];
    
    DEBUG_PRINT("alg=%p, key_a=%p, dat=%p, len=%d hash=%p hashlen=%d ",
	alg, key_a, dat, len, hash, hashlen);
    
    /* SHA1(K XOR ipad, text) */
    sha1_init(&mctx);
    sha1_update(&mctx, ctx->k_ipad, SHA1_HMAC_BLOCK_SIZE);
    sha1_update(&mctx, dat, len);    
    sha1_final(&mctx, digest); 

    /* SHA1(K XOR opad, digest) */
    sha1_init(&mctx);                 
    sha1_update(&mctx, ctx->k_opad, SHA1_HMAC_BLOCK_SIZE);     
    sha1_update(&mctx, digest, sizeof(digest));     
    
    if (hashlen == SHA1_DIGEST_SIZE)
	sha1_final(&mctx, hash);
    else
    {
	__u8 hash_buf[SHA1_DIGEST_SIZE];
	sha1_final(&mctx, hash_buf);
	memcpy(hash, hash_buf, hashlen);
    }

   return 0;
}

EXPORT_SYMBOL(_des_twinpass_set_key);
EXPORT_SYMBOL(_1des_twinpass_cbc_encrypt);
EXPORT_SYMBOL(_3des_twinpass_cbc_encrypt);
EXPORT_SYMBOL(_aes_twinpass_set_key);
EXPORT_SYMBOL(_aes_twinpass_cbc_encrypt);
EXPORT_SYMBOL(_md5_twinpass_hmac_set_key);
EXPORT_SYMBOL(_md5_twinpass_hmac_hash);
EXPORT_SYMBOL(_sha1_twinpass_hmac_set_key);
EXPORT_SYMBOL(_sha1_twinpass_hmac_hash);
