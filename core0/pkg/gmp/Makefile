RGSRC=../../
include $(RGSRC)/envir.mak

ifdef IS_BUILDDIR

EXPORT_HEADERS_DIR=gmp
EXPORT_HEADERS+=gmp.h

CONFIG_FLAGS = --host=$(TARGET_MACHINE) --target=$(TARGET_MACHINE)
ifeq ($(ARCH),arm)
  CONFIG_FLAGS = --host=arm-linux --target=arm-linux --build=i386-linux
endif
ifeq ($(ARCH),mips)
  CONFIG_FLAGS = --host=mips-linux --target=mips-linux --build=i386-linux
endif
ifeq ($(ARCH),i386)
  CONFIG_FLAGS = --host=i386-linux --target=i386-linux --build=i386-linux
endif
ifeq ($(ARCH),um)
  CONFIG_FLAGS = --host=i386-linux --target=i386-linux --build=i386-linux
endif
ifeq ($(ARCH),ppc)
  CONFIG_FLAGS = --host=powerpc-linux --target=powerpc-linux --build=powerpc-linux
endif
gmpMakefile=gmp.created_by_configure.mak
export gmpMakefile

all: archconfig dummy
	if test -f $(gmpMakefile); then \
	$(MAKE) -f $(gmpMakefile) CFLAGS="$(call FIX_VPATH_CFLAGS,$(CFLAGS)) \
	   $(if $(CONFIG_ULIBC),$(RG_ULIBC_CFLAGS),$(RG_GLIBC_CFLAGS))" \
	   LDFLAGS="$(LDFLAGS)" \
	AR=$(AR) RANLIB=$(RANLIB) Makefile=$(gmpMakefile);\
	fi

archconfig: .configure export_headers dummy

#XXX a hack so dep will not fail.
dep:
run_unittests:

export_headers:
	$(MKDIR) $(BUILDDIR)/pkg/include/$(EXPORT_HEADERS_DIR)
	$(foreach f,$(EXPORT_HEADERS),$(RG_LN) $(PWD_SRC)/$f \
	  $(BUILDDIR)/pkg/include/$(EXPORT_HEADERS_DIR)/$f)

.configure: 
ifndef DIST
	@echo 'error - give param DIST' && false
endif	
	@touch .top_level
	./configure $(CONFIG_FLAGS)
	@rm -f .top_level
	@touch .configure

clean:
	if test -f $(gmpMakefile); then \
	$(MAKE) -f $(gmpMakefile) Makefile=$(gmpMakefile) clean; \
	fi
	@rm -f .configure .depend

ifdef CONFIG_DYN_LINK
  RAMDISK_LIB_FILES=libgmp.so.2.0.2
endif
include $(RGSRC)/copy_db.mak

ifdef CONFIG_DYN_LINK
ramdisk::
	$(call RAMDISK_LN_FUNC,libgmp.so.2.0.2,$(RAMDISK_RW_LIB_DIR)/libgmp.so.2)
endif

force:
.PNONY: check install install-bsdmp install-info-files install-strip uninstall
.PHONY: all clean archconfig ramdisk

else # IS_BUILDDIR
# XXX This code is taken from rg.mak. Its a hack until this Makefile will be
# writen properly.
# 
# This hack enables compiling from within the pkg/gmp directory.
# In the normal flow of compilation we will always enter the 'if' side of
# 'ifdef IS_BUILDDIR'. Only when doing 'cd pkg/gmp ; make' we will enter here
# (the 'else' side of 'ifdef IS_BUILDDIR').
#
# IMPORTANT: just doing 'make' will not work,
# only 'make all <or some other target>' will work

%:
	@echo "pwd: `/bin/pwd` PWD:$(PWD) [$@:$^]"
	mkdir -p $(BUILDDIR)/pkg/gmp
	make -C $(BUILDDIR)/pkg/gmp $(MAKECMDGOALS) \
	  RGSRC=$(RGSRC)
endif # IS_BUILDDIR
