/****************************************************************************
 *  Copyright (c) 2002 Jungo LTD. All Rights Reserved.
 * 
 *  rg/pkg/freeswan/klips/net/ipsec/alg/ipsec_alg_sha1_twinpass.c
 *
 *  Developed by Jungo LTD.
 *  Residential Gateway Software Division
 *  www.jungo.com
 *  info@jungo.com
 *
 *  This file is part of the OpenRG Software and may not be distributed,
 *  sold, reproduced or copied in any way.
 *
 *  This copyright notice should not be removed
 *
 */

#include <linux/config.h>
#include <linux/version.h>

/*	
 *	special case: ipsec core modular with this static algo inside:
 *	must avoid MODULE magic for this file
 */
#if CONFIG_IPSEC_MODULE && CONFIG_IPSEC_ALG_SHA1
#undef MODULE
#endif

#include <linux/module.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/string.h>
#include <ipsec_log.h>

/* Check if __exit is defined, if not null it */
#ifndef __exit
#define __exit
#endif

#include "ipsec_alg.h"

static int test=1;
MODULE_PARM(test, "i");
static int excl=0;
MODULE_PARM(excl, "i");

#define AH_SHA 3

#define SHA1_DIGEST_SIZE 20
#define SHA1_HMAC_BLOCK_SIZE 64

typedef struct sha1_hmac_context {
    u8 k_opad[SHA1_HMAC_BLOCK_SIZE + 1];
    u8 k_ipad[SHA1_HMAC_BLOCK_SIZE + 1];
} sha1_hmac_context;

extern int _sha1_twinpass_hmac_set_key(struct ipsec_alg_auth *alg, __u8 * key_a,
	const __u8 * key, int keylen);
extern int _sha1_twinpass_hmac_hash(struct ipsec_alg_auth *alg, __u8 * key_a, 
	const __u8 * dat, int len, __u8 * hash, int hashlen);

static struct ipsec_alg_auth ipsec_alg_SHA1_twinpass = {
	ixt_version:	IPSEC_ALG_VERSION,
	ixt_module:	THIS_MODULE,
	ixt_refcnt:	ATOMIC_INIT(0),
	ixt_alg_type:	IPSEC_ALG_TYPE_AUTH,
	ixt_alg_id: 	AH_SHA,
	ixt_name:	"twinpass_sha1",
	ixt_blocksize:	SHA1_HMAC_BLOCK_SIZE,
	ixt_keyminbits:	160,
	ixt_keymaxbits:	160,
	ixt_a_keylen:	160/8,
	ixt_a_ctx_size:	sizeof(sha1_hmac_context),
	ixt_a_hmac_set_key: _sha1_twinpass_hmac_set_key,  
	ixt_a_hmac_hash: _sha1_twinpass_hmac_hash,
};

IPSEC_ALG_MODULE_INIT( ipsec_sha1_init )
{
	int ret, test_ret;
	if (excl) ipsec_alg_SHA1_twinpass.ixt_state |= IPSEC_ALG_ST_EXCL;
	ret=register_ipsec_alg_auth(&ipsec_alg_SHA1_twinpass);
	ipsec_log("ipsec_sha1_init(alg_type=%d alg_id=%d name=%s): ret=%d\n", 
			ipsec_alg_SHA1_twinpass.ixt_alg_type, 
			ipsec_alg_SHA1_twinpass.ixt_alg_id, 
			ipsec_alg_SHA1_twinpass.ixt_name, 
			ret);
	if (ret==0 && test) {
		test_ret=ipsec_alg_test(
				ipsec_alg_SHA1_twinpass.ixt_alg_type,
				ipsec_alg_SHA1_twinpass.ixt_alg_id, 
				test);
		ipsec_log("ipsec_sha1_init(alg_type=%d alg_id=%d): "
		    "test_ret=%d\n", 
		                ipsec_alg_SHA1_twinpass.ixt_alg_type, 
				ipsec_alg_SHA1_twinpass.ixt_alg_id, 
				test_ret);
	}
	return ret;
}

IPSEC_ALG_MODULE_EXIT( ipsec_sha1_fini )
{
	unregister_ipsec_alg_auth(&ipsec_alg_SHA1_twinpass);
	return;
}

