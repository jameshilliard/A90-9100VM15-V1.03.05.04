RGSRC=../../../../../..
include $(RGSRC)/envir.mak

MOD_TARGET=serial.o spi_mdio.o
MOD_2_STAT=$(MOD_TARGET)
O_OBJS_serial.o=serial_core.o ifx_asc.o
OX_OBJS_spi_mdio.o=westell_spi_mdio.o
ifdef CONFIG_HW_MOCA_WAN
  MOD_CFLAGS+=-DDUAL_SPI_MDIO
endif

ifdef CONFIG_WSTL_DUAL_SPI_MDIO
  MOD_CFLAGS+=-DDUAL_SPI_MDIO
endif

INTERNAL_HEADERS=serial_core.h

include $(RGMK)
