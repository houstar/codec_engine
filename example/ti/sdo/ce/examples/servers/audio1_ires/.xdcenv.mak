#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/muti/ezsdk/component-sources/codec_engine_3_22_01_06/examples/ti/sdo/ce/examples/servers/audio1_ires/../../../../../..;/home/muti/ezsdk/component-sources/xdctools_3_23_03_53/packages;/home/muti/ezsdk/component-sources/xdais_7_22_00_03/packages;/home/muti/ezsdk/component-sources/bios_6_33_05_46/packages;/home/muti/ezsdk/component-sources/linuxutils_3_22_00_02/packages;/home/muti/ezsdk/component-sources/syslink_2_20_02_20/packages;/home/muti/ezsdk/component-sources/osal_1_22_01_09/packages;/home/muti/ezsdk/component-sources/framework_components_3_22_01_07/packages;/home/muti/ezsdk/component-sources/edma3lld_02_11_05_02/packages;/home/muti/ezsdk/component-sources/ipc_1_24_03_32/packages;/home/muti/ezsdk/component-sources/codec_engine_3_22_01_06//packages
override XDCROOT = /home/muti/ezsdk/component-sources/xdctools_3_23_03_53
override XDCBUILDCFG = /home/muti/ezsdk/component-sources/codec_engine_3_22_01_06/examples/ti/sdo/ce/examples/servers/audio1_ires/_config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = DEVICE="TI816X" PROGRAMS="APP_CLIENT DSP_SERVER" GPPOS="LINUX_GCC" ti.targets.C64P="" ti.targets.C674="/home/muti/ezsdk/dsp-devkit/cgt6x_7_3_4" ti.targets.arm.elf.M3="" ti.targets.elf.C64P="" ti.targets.elf.C64T="" ti.targets.elf.C66="" ti.targets.elf.C674="/home/muti/ezsdk/dsp-devkit/cgt6x_7_3_4" gnu.targets.arm.GCArmv5T=";LONGNAME=/home/muti/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-gcc;profiles.release.compileOpts.copts=-O2 -ffunction-sections" google.targets.arm.Bionic=";LONGNAME=;libcDir=;fsDir="
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/muti/ezsdk/component-sources/codec_engine_3_22_01_06/examples/ti/sdo/ce/examples/servers/audio1_ires/../../../../../..;/home/muti/ezsdk/component-sources/xdctools_3_23_03_53/packages;/home/muti/ezsdk/component-sources/xdais_7_22_00_03/packages;/home/muti/ezsdk/component-sources/bios_6_33_05_46/packages;/home/muti/ezsdk/component-sources/linuxutils_3_22_00_02/packages;/home/muti/ezsdk/component-sources/syslink_2_20_02_20/packages;/home/muti/ezsdk/component-sources/osal_1_22_01_09/packages;/home/muti/ezsdk/component-sources/framework_components_3_22_01_07/packages;/home/muti/ezsdk/component-sources/edma3lld_02_11_05_02/packages;/home/muti/ezsdk/component-sources/ipc_1_24_03_32/packages;/home/muti/ezsdk/component-sources/codec_engine_3_22_01_06//packages;/home/muti/ezsdk/component-sources/xdctools_3_23_03_53/packages;../../../../../..
HOSTOS = Linux
endif
