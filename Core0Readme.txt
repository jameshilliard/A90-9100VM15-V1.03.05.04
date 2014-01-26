UltraLine Series 3 Build Instructions
How to install and build the open source package

   1. Download both the toolchain and core0 files.
   2. As root untar the toolchain.
          * tar jxf toolchain.tar.bz2
   3. Install toolchain
          * cd toolchain
          * ./install.sh
   4. Untar core0 source
          * tar jxf core0.tar.bz2
   5. Make the source
          * cd {Source Directory}
          * make config DIST=WSTL_ULS3_DUAL_MOCA_USB_64 CONFIG_RG_GPL=y LIC=../jpkg_westell_gpl.lic
          * make
