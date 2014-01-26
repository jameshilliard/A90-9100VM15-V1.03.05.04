RGSRC=../../../../../..
include $(RGSRC)/envir.mak

MOD_$(CONFIG_CRYPTO_DANUBE)=twinpass_crypto.o

MOD_TARGET+=$(MOD_m) $(MOD_y)
MOD_2_STAT=$(MOD_y)
RAMDISK_MODULES_FILES+=$(MOD_m)

O_OBJS_twinpass_crypto.o+=$(if $(CONFIG_CRYPTO_DEV_INCAIP1_DES), ifxdeu-des.o)
O_OBJS_twinpass_crypto.o+=$(if $(CONFIG_CRYPTO_DEV_INCAIP1_AES), ifxdeu-aes.o)
O_OBJS_twinpass_crypto.o+=$(if $(CONFIG_CRYPTO_DEV_INCAIP2_DES), ifxdeu-des.o)
O_OBJS_twinpass_crypto.o+=$(if $(CONFIG_CRYPTO_DEV_INCAIP2_AES), ifxdeu-aes.o)
O_OBJS_twinpass_crypto.o+=$(if $(CONFIG_CRYPTO_DEV_INCAIP2_SHA1), ifxdeu-sha1.o)
O_OBJS_twinpass_crypto.o+=$(if $(CONFIG_CRYPTO_DEV_INCAIP2_MD5), ifxdeu-md5.o)
O_OBJS_twinpass_crypto.o+=$(if $(CONFIG_CRYPTO_DEV_DANUBE_DES), ifxdeu-des.o)
O_OBJS_twinpass_crypto.o+=$(if $(CONFIG_CRYPTO_DEV_DANUBE_AES), ifxdeu-aes.o)
O_OBJS_twinpass_crypto.o+=$(if $(CONFIG_CRYPTO_DEV_DANUBE_SHA1), ifxdeu-sha1.o)
O_OBJS_twinpass_crypto.o+=$(if $(CONFIG_CRYPTO_DEV_DANUBE_MD5), ifxdeu-md5.o)
O_OBJS_twinpass_crypto.o+=$(if $(CONFIG_CRYPTO_DEV_DANUBE_DMA), ifxdeu-dma.o)
O_OBJS_twinpass_crypto.o+=ifxdeu-generic.o
OX_OBJS_twinpass_crypto.o+=sec_api.o

include $(RGMK)
