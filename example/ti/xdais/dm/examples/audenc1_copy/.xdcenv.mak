#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/houstar/DM8168/code/codec_engine/example/ti/sdo/ce/examples/codecs/audenc1_copy/../../../../../..;/home/houstar/ezsdk//component-sources/xdctools_3_23_03_53/packages;/home/houstar/ezsdk/component-sources/xdais_7_22_00_03/packages;/home/houstar/ezsdk/component-sources/bios_6_33_05_46/packages;/home/houstar/ezsdk/component-sources/linuxutils_3_22_00_02/packages;/home/houstar/ezsdk/component-sources/syslink_2_20_02_20/packages;/home/houstar/ezsdk/component-sources/osal_1_22_01_09/packages;/home/houstar/ezsdk/component-sources/framework_components_3_22_01_07/packages;/home/houstar/ezsdk/component-sources/edma3lld_02_11_05_02/packages;/home/houstar/ezsdk/component-sources/ipc_1_24_03_32/packages;/home/houstar/ezsdk/component-sources/codec_engine_3_22_01_06//packages
override XDCROOT = /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53
override XDCBUILDCFG = /home/houstar/ezsdk/component-sources/codec_engine_3_22_01_06/codec_engine.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = DEVICE="TI816X" PROGRAMS="APP_CLIENT DSP_SERVER" GPPOS="LINUX_GCC" ti.targets.C64P="" ti.targets.C674="/home/houstar/ezsdk/dsp-devkit/cgt6x_7_3_4" ti.targets.arm.elf.M3="" ti.targets.elf.C64P="" ti.targets.elf.C64T="" ti.targets.elf.C66="" ti.targets.elf.C674="/home/houstar/ezsdk/dsp-devkit/cgt6x_7_3_4" gnu.targets.arm.GCArmv5T=";LONGNAME=/home/houstar/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-gcc;profiles.release.compileOpts.copts=-O2 -ffunction-sections" google.targets.arm.Bionic=";LONGNAME=;libcDir=;fsDir="
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/houstar/DM8168/code/codec_engine/example/ti/sdo/ce/examples/codecs/audenc1_copy/../../../../../..;/home/houstar/ezsdk//component-sources/xdctools_3_23_03_53/packages;/home/houstar/ezsdk/component-sources/xdais_7_22_00_03/packages;/home/houstar/ezsdk/component-sources/bios_6_33_05_46/packages;/home/houstar/ezsdk/component-sources/linuxutils_3_22_00_02/packages;/home/houstar/ezsdk/component-sources/syslink_2_20_02_20/packages;/home/houstar/ezsdk/component-sources/osal_1_22_01_09/packages;/home/houstar/ezsdk/component-sources/framework_components_3_22_01_07/packages;/home/houstar/ezsdk/component-sources/edma3lld_02_11_05_02/packages;/home/houstar/ezsdk/component-sources/ipc_1_24_03_32/packages;/home/houstar/ezsdk/component-sources/codec_engine_3_22_01_06//packages;/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages;../../../../..
HOSTOS = Linux
endif
