# 
#  Copyright (c) 2012, Texas Instruments Incorporated
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  *  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#  *  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  *  Neither the name of Texas Instruments Incorporated nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
#
#  ======== video1_copy.rtcfg_remote_evm3530_linux/makefile ========
#  GNUmake-based makefile
#


include ${EZSDK}/Rules.make

#
# EXAMPLES_ROOTDIR should be set to the examples directory under your
# CE_INSTALL_DIR.
#
EXAMPLES_ROOTDIR ?= $(CURDIR)/../../../../../../../..
#
#  Include xdcpaths.mak if it exists. This file defines package paths and
#  build rules.  If xdcpaths.mak does not exist the variables must be set on
#  the command line.
#
PROGRAM = armlivemedia

-include $(EXAMPLES_ROOTDIR)/xdcpaths.mak

#
#  ======== define all the repositories needed by this example ========
#
XDCTOOLS_REPO   ?= $(XDC_INSTALL_DIR)/packages
CE_REPO         ?= $(CE_INSTALL_DIR)/packages
SUFFIX          ?= v5T

#
#  Make sure CE_INSTALL_DIR was set properly. By using "-include" to include
#  xdcpaths.mak, no error message will be given if xdcpaths.mak is not found,
#  and in that case, the user must have set the environment variables, for
#  example, on the command line.
# 
ERRMSG = which is invalid (could not find file "$(TEST_FILE)"). Set this in <codec engine examples>/xdcpaths.mak! If you intended to use xdcpaths.mak, also please make sure that EXAMPLES_ROOTDIR is set correctly in this makefile. See the build documentation to correct this error.
TEST_FILE := $(CE_REPO)/ti/sdo/ce/package.xdc
ifeq ($(wildcard $(TEST_FILE)),)
    $(error CE_INSTALL_DIR is set to "$(CE_INSTALL_DIR)", $(ERRMSG))
endif


REPO_PATH = \
	$(XDCTOOLS_REPO) \
	$(CE_REPO) \
	$(LINK_INSTALL_DIR)/packages \
	$(XDAIS_INSTALL_DIR)/packages \
	$(FC_INSTALL_DIR)/packages \
	$(CMEM_INSTALL_DIR)/packages \
	$(EXAMPLES_ROOTDIR) 
#$(BIOS_INSTALL_DIR)/packages


CE_APPS_DIR ?= $(EXAMPLES_ROOTDIR)/ti/sdo/ce/examples/apps

#
#  ======== common defines ========
#
ALL.defs =
ALL.incs = -I. $(addprefix -I,$(REPO_PATH))
ALL.libs = $(addprefix -L,$(REPO_PATH))

EXTLIBS_$(PROFILE) = \
	$(CE_APPS_DIR)/rtcfg/osal/$(DEV)/lib/osal.a \
	$(CE_APPS_DIR)/rtcfg/osal/$(DEV)/lib/cstubs.a

#
#  ======== standard macros ========
#
ECHO    ?= echo
MKDIR   ?= mkdir
RM      ?= rm -f
RMDIR   ?= rm -rf


#$(shell $(MKDIR) -p bin/$(PLAT)/$(PROFILE))
#$(shell $(MKDIR) -p bin/$(PLAT)/$(PROFILE)/gsoap)
#$(shell $(MKDIR) -p bin/$(PLAT)/$(PROFILE)/xml)
#$(shell $(MKDIR) -p bin/$(PLAT)/$(PROFILE)/rtmp)
#$(shell $(MKDIR) -p bin/$(PLAT)/$(PROFILE)/rtp)


ifneq (clean,$(MAKECMDGOALS))
ifneq (,$(PROFILE))
ifeq (,$(wildcard bin/$(PLAT)/$(PROFILE)))
$(shell $(MKDIR) -p bin/$(PLAT)/$(PROFILE))
endif
endif
endif

.PHONY: nothing

#
#  ======== toolchain macros ========
#  Note: You do not need to set CGTOOLS_V5T and CC_V5T if they have been
#  set in xdcpaths.mak.
#
CGTOOLS_V5T ?= <__your_CGTOOLS_V5T__>
CC_V5T ?= <__your_CC_V5T__>
CC = $(CGTOOLS_V5T)/$(CC_V5T)
CXX		:= $(CSTOOL_PREFIX)g++
AR		:= $(CSTOOL_PREFIX)ar
RANLIB	:= $(CSTOOL_PREFIX)ranlib

#
#   =========== OMX =================
#

CCPROFILE.debug = -g -D_DEBUG_=1 
CCPROFILE.release = -O3

INCLUDES := -I. $(addprefix -I,$(REPO_PATH)) \
	-I${EXAMPLES_ROOTDIR}/../../common/AbleMp4lib \
	-I${EXAMPLES_ROOTDIR}/../../common/MP4FileReader \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/tinyxml \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/activemq-cpp-library-3.8.3/install/include/activemq-cpp-3.8.3/	\
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/libevent-2.0.21-stable/install/include \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/libevent-2.0.21-stable/install/include/event2 \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1 \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/include \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/lib/mp4v2 \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/lib/mp4av \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/lib.avi \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/common/video/libmpeg32 \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/lib/ismacryp \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/lib/mpeg2ps \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/HikVision/incCn \
	-I${EXAMPLES_ROOTDIR}/../../ThirdParty/mtg \
	-I${EXAMPLES_ROOTDIR}/../../common/ACCHandle \
 	-I$(EZSDK)linux-devkit/arm-none-linux-gnueabi/usr/include \
	-I$(EZSDK)linux-devkit/arm-none-linux-gnueabi/usr/src/linux-2.6.37-psp04.04.00.01-headers/include \
	-I$(EZSDK)linux-devkit/arm-none-linux-gnueabi/usr/include \
	-I$(EZSDK)linux-devkit/arm-none-linux-gnueabi/usr/include/cairo \
    	-I$(EZSDK)component-sources/omx_05_02_00_48/src \
    	-I$(EZSDK)component-sources/omx_05_02_00_48/include \
	-I$(EZSDK)component-sources/omx_05_02_00_48/src/ti/omx/domx \
	-I$(EZSDK)component-sources/syslink_2_20_02_20/packages \
	-I$(EZSDK)component-sources/bios_6_33_05_46/packages \
	-I$(EZSDK)component-sources/osal_1_22_01_09/packages \
	-I$(EZSDK)component-sources/xdais_7_22_00_03/packages \
	-I$(EZSDK)component-sources/uia_1_01_01_14/packages \
	-I$(EZSDK)component-sources/c674x_aaclcenc_01_00_01_00_elf/packages 

CFLAGS = -fPIC -Wall -fno-strict-aliasing $(CCPROFILE.$(PROFILE)) \
    -Wunused -g -fno-strict-aliasing -MD -MF \
	-march=armv5t -Dfar= -D_DEBUG_=1 -DMULTICHANNEL_OPT=1 \
	-Dxdc_target_name__=GCArmv5T -Dxdc_target_types__=gnu/targets/arm/std.h \
	-Dxdc_bld__profile_debug  -Dxdc_bld__vers_1_0_4_3_3  \
	-DGlobal_appTerminate=0 -D_VIDEO_M3_DYNAMIC_CONFIG \
	-DGlobal_GrpxDssMsgHeapId=6 -DGlobal_TilerHeapId=7 \
	-D_LOCAL_CORE_a8host_     \
	-D_REMOTE_omxbase_ -D_REMOTE_venc_ -D_REMOTE_vdec_ \
	-D_REMOTE_server_ -D_REMOTE_scheduler_ \
	-D_REMOTE_domx_ -D_REMOTE_domx_delegates_shmem_ -D_REMOTE_omxcore_ \
	-D_REMOTE_omxbase_ -D_REMOTE_vfcc_ -D_REMOTE_vfpc_ \
	-D_REMOTE_vfdc_ -D_REMOTE_ctrl_ -D_REMOTE_domx_ \
	-D_REMOTE_domx_ -D_REMOTE_domx_delegates_shmem_ \
	-D_REMOTE_omxcore_ -D_BUILD_omxbase_ \
	-D_BUILD_venc_ -D_BUILD_vdec_ -D_BUILD_server_ -D_BUILD_scheduler_ -D_BUILD_domx_ \
	-D_BUILD_domx_delegates_shmem_ -D_BUILD_omxcore_ -D_BUILD_omxbase_ \
	-D_BUILD_domx_ -D_BUILD_domx_delegates_shmem_ -D_BUILD_omxcore_  \
	-D_BUILD_domx_delegates_shmem_ -D_BUILD_omxcore_ \
	-D_BUILD_omxbase_ -D_BUILD_vfcc_ -D_BUILD_vfpc_ \
	-D_BUILD_vfdc_ -D_BUILD_ctrl_ -D_BUILD_domx_ \
	-D_BUILD_domx_delegates_shmem_ -D_BUILD_omxcore_  \
	-DMAX_RESOLUTION_HD \
	-DDOMX_CORE_REMOTEDUCATIHOST -DVC_APPS -DCODEC_H264ENC -DCODEC_H264DEC \
	-DTI_816X_BUILD  -D_FILE_OFFSET_BITS=64 \
	-DPLATFORM_EVM_SI -DADD_FBDEV_SUPPORT

CPPFLAGS = -Dxdc_target_name__=GCArmv5T \
    -Dxdc_target_types__=gnu/targets/arm/std.h

#${EXAMPLES_ROOTDIR}/../../ThirdParty/activemq-cpp-library-3.8.3/install/lib/libactivemq-cpp.a 	\
#${EXAMPLES_ROOTDIR}/../../ThirdParty/apr-1.5.1/install/lib/libapr-1.a	\

LNKPROFILE.debug = -g
LNKPROFILE.release =
LDFLAGS = $(LNKPROFILE.$(PROFILE)) \
		-Xlinker --start-group \
		${EXAMPLES_ROOTDIR}/../../ThirdParty/tinyxml/libtinyxml.a \
		${EXAMPLES_ROOTDIR}/../../ThirdParty/libevent-2.0.21-stable/install/lib/libevent.a \
		${EXAMPLES_ROOTDIR}/../../common/AbleMp4lib/libAblemp4.a \
		${EXAMPLES_ROOTDIR}/../../common/MP4FileReader/libMP4FileReader.a \
		${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/install/libismacryp.a \
		${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/install/libmp4.a \
		${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/install/libmp4av.a \
		${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/install/libmpeg4ip_gnu.a \
		${EXAMPLES_ROOTDIR}/../../ThirdParty/mpeg4ip-1.5.0.1/install/libmp4v2.a \
		${EXAMPLES_ROOTDIR}/../../common/ACCHandle/libACCHandle.a \
		${EZSDK}component-sources/omx_05_02_00_48/lib/omxcore.av5T \
		${EZSDK}component-sources/omx_05_02_00_48/lib/memcfg.av5T \
		${EZSDK}component-sources/omx_05_02_00_48/lib/domx.av5T \
		${EZSDK}component-sources/omx_05_02_00_48/lib/domx_delegates_shmem.av5T \
		${EZSDK}component-sources/omx_05_02_00_48/lib/timmosal.av5T \
		${EZSDK}component-sources/omx_05_02_00_48/lib/omxcfg.av5T \
		${EZSDK}component-sources/osal_1_22_01_09/packages/linuxdist/build/lib/osal.a \
		${EZSDK}component-sources/osal_1_22_01_09/packages/linuxdist/cstubs/lib/cstubs.a \
		${EZSDK}component-sources/framework_components_3_22_01_07/packages/ti/sdo/rcm/lib/debug/rcm_syslink.av5T \
		${EZSDK}component-sources/framework_components_3_22_01_07/packages/ti/sdo/fc/memutils/lib/release/memutils.av5T \
		${EZSDK}component-sources/osal_1_22_01_09/packages/ti/sdo/xdcruntime/linux/lib/debug/osal_linux_470.av5T \
		${EZSDK}component-sources/framework_components_3_22_01_07/packages/ti/sdo/fc/global/lib/debug/fcsettings.av5T \
		${EZSDK}component-sources/syslink_2_20_02_20/packages/ti/syslink/lib/syslink.a_debug \
		${EZSDK}component-sources/linuxutils_3_22_00_02/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV \
		${EZSDK}component-sources/uia_1_01_01_14/packages/ti/uia/linux/lib/servicemgr.a \
		-Wl,--whole-archive -Wl,--no-whole-archive -lc \
		-rdynamic -ldl -lpthread -lrt \
		-Xlinker --end-group \
		-lasound -lcairo	-lpng	-lfreetype	-lfontconfig	\
		-lXrender	-lX11	-lxcb	-lpixman-1	-lexpat	-lXau	\
		-lpthread-stubs	-lz \
		-L${EZSDK}linux-devkit/arm-none-linux-gnueabi/usr/lib/	\
		-lactivemq-cpp \
		-L${EXAMPLES_ROOTDIR}/../../ThirdParty/activemq-cpp-library-3.8.3/install/lib/ \
		-lapr-1	\
		-L${EXAMPLES_ROOTDIR}/../../ThirdParty/apr-1.5.1/install/lib/



#
#  ======== example sources ========
#
#  This example is built with the following source files:
#
#  $(CE_REPO)/ti/sdo/ce/utils/rtcfg/
#      rtcfg.c
#      rtcfg_fcinit.c	
#
#      The source files in $(CE_REPO)/ti/sdo/ce/utils/rtcfg are run-time
#      configuration files.  These files do not need to be modified to run this
#      example. If you want to modify the configuration in any of these files,
#      copy it to this directory and modify as needed.
#
#  $(EXAMPLES_ROOTDIR)/ti/sdo/ce/examples/apps/video1_copy/
#      app.c
#      main_hlos_remote.c
#
#      These are the same source files used to build the XDC configurable
#      version of this app. They do not need to be modified for the run-time
#      configuration example.
#
#  $(CURDIR)/
#      app_config.c
#      evm3530_config_all.c
#
#      The file, app_config.c, contains algorithm and engine tables needed
#      by the app. The file, evm3530_config_all.c, contains platform specific
#      configuration.
#
#  The vpath variable below sets the search path for the source files.
#
APP.srcs =                      \
	rtcfg.c                     \
	rtcfg_fcinit.c		        \
        app_config.c            \
        evmTI816X_config_all.c  
        

APP.objs = $(addprefix bin/$(PLAT)/$(PROFILE)/, \
    $(patsubst %.c,%.o$(SUFFIX),$(APP.srcs)))

                
APP_CPP.srcs = app.cpp	\
			main_native.cpp \
			log.cpp	\
			bmpfile.cpp	\
			codeConverter.cpp	\
			colorConvert.cpp	\
			commonUtils.cpp	\
			liveCourseUtils.cpp	\
			vgaState.cpp	\
			liveCourseManager.cpp	\
			subVideoManager.cpp	\
			classWxManager.cpp	\
			courseWxManager.cpp	\
			AssistCommandHandle.cpp	\
			Function.cpp	\
			KernelBoardCommandHandle.cpp	\
			udp.cpp	\
			captureVga.cpp	\
			captureSDIVideo.cpp \
			sdiVideoProviderSystem.cpp \
			CardInfo.cpp	\
			audioCapture.cpp \
			HikVideo.cpp	\
			streamVideoManager.cpp	\
			vgaProviderSystem.cpp	\
			audio_capture.cpp   \
			audio_mixer.cpp     \
			audio_mix_linear2ulaw_table.cpp \
			audio_playback.cpp  \
			codec_engine.cpp    \
			fifo.cpp	\
			VideoFactory.cpp	\
			audioEncodeFactory.cpp	\
			audioEncodeOMX.cpp	\
			baseMode.cpp	\
			cameraBufferPool.cpp	\
			DecodeUtils.cpp	\
			dualScreenMode_IPC.cpp	\
			dualScreenMode_SDI.cpp	\
			edma.cpp	\
			framePool.cpp	\
			HDMIDisplayManager.cpp	\
			interactionMode_IPC.cpp	\
			interactionMode_SDI.cpp	\
			membufUtils.cpp	\
			microMode_IPC.cpp	\
			microMode_SDI.cpp	\
			mixtureMode_IPC.cpp	\
			mixtureMode_SDI.cpp	\
			movieHeaderTail.cpp	\
			movieMode_IPC.cpp	\
			movieMode_SDI.cpp	\
			mp4Output.cpp	\
			Mp4Player.cpp	\
			msgq.cpp	\
			mtgClient.cpp	\
			mtgInit.cpp	\
			Mutex.cpp	\
			OMXComponentUtils.cpp	\
			osdManager.cpp	\
			recordModeFactory.cpp	\
			resourceMode_IPC.cpp	\
			resourceMode_SDI.cpp	\
			rtmpHandle.cpp	\
			rtmpManager.cpp	\
			semp.cpp	\
			shareMem.cpp	\
			v4lDisplay.cpp	\
			setting.cpp	\
			timer.cpp	\
			liveMediaState.cpp	\
			kernel.cpp	\
			mtgFunc.cpp	\
			hikFunc.cpp	\
			modelStatus.cpp	\
			recordList.cpp	\
			monitorSender.cpp	\
			json_reader.cpp	\
			json_value.cpp	\
			json_writer.cpp	
			
			
			


APP_CPP.objs := $(addprefix bin/$(PLAT)/$(PROFILE)/, \
    $(patsubst %.cpp,%.o$(SUFFIX),$(APP_CPP.srcs)))

#
#  The vpath variable lets us maintain some of the same sources for both XDC
#  build and gmake build, and use default run-time configuration files.
#

APP_PATH := $(CE_APPS_DIR)/armlivemedia

vpath %.c $(APP_PATH) $(CE_REPO)/ti/sdo/ce/utils/rtcfg
vpath %.cpp $(APP_PATH) $(APP_PATH)/utils $(APP_PATH)/liveCourse $(APP_PATH)/Command $(APP_PATH)/core $(APP_PATH)/KVideo $(APP_PATH)/KVideo/audio $(APP_PATH)/web $(APP_PATH)/monitor
#
#  ======== linker command files ========
#
#  Set up path to find linker command files, and which linker command
#  files will be used (eg, debug or release).
#

#  Path for linker command files. We use several linker command files,
#  keeping those containing Codec Engine and Framework Components libraries
#  in a common area where they can be used by multiple apps. The linker
#  command files in this directory just contain the codec libraries needed
#  by this app.
#
#  The common linker command files, in ti/sdo/ce/utils/rtcfg/linux/<platform>,
#  shouldn't need modification, but in the chance that they did, they could be
#  copied to this directory and modified as needed. The vpath will cause
#  any modified linker command files to be found first.
#
vpath %.cmd $(CE_REPO)/ti/sdo/ce/utils/rtcfg/linux/evmTI816X

LINKER_FILE = # linker_$(PROFILE).cmd - No local codecs, so we don't use this.
CE_LINKER_FILE = ce_remote_$(PROFILE).cmd
FC_LINKER_FILE = fc_$(PROFILE).cmd

#
#  ======== compute build goals from DEVICE and PROGRAMS ========
#  Don't attempt to build unless DEVICE = OMAP3530, PROGRAMS = APP_CLIENT,
#  and GPPOS is Linux.
#
ifneq (,$(findstring TI816X,$(DEVICE)))
ifneq (,$(findstring APP_CLIENT,$(PROGRAMS)))
ifneq (,$(findstring LINUX,$(GPPOS)))
GOALS := $(GOALS) TI816X_APP_CLIENT
endif
endif
endif

ifeq (,$(GOALS))
GOALS = nothing
endif

#
#  ======== make commands ========
#
all:
	$(MAKE) $(GOALS)

TI816X_APP_CLIENT:
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	@$(MAKE) PLAT=ti_platforms_evmTI816X DEV=debug\
            PROFILE=debug TARGET=GCArmv5T \
            bin/ti_platforms_evmTI816X/debug/$(PROGRAM).xv5T
	@$(MAKE) PLAT=ti_platforms_evmTI816X DEV=release\
            PROFILE=release TARGET=GCArmv5T \
            bin/ti_platforms_evmTI816X/release/$(PROGRAM).xv5T


nothing:
	@$(ECHO) "Nothing to build for the given devices and programs"
	@$(ECHO) "    DEVICE=\"$(DEVICE)\""
	@$(ECHO) "    PROGRAMS=\"$(PROGRAMS)\""

#
#  ======== build rules ========
#
bin/$(PLAT)/$(PROFILE)/$(PROGRAM).x$(SUFFIX): $(APP.objs) $(APP_CPP.objs) $(EXTLIBS_$(PROFILE)) $(CE_LINKER_FILE) $(LINKER_FILE) $(FC_LINKER_FILE)
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	$(CXX) -o $@ $^ $(EXTLIBS_$(PROFILE)) $(LDFLAGS) $(ALL.defs) $(ALL.libs)
	
#$(shell cp bin/ti_platforms_evmTI816X/debug/$(PROGRAM).x$(SUFFIX) ./)


.SECONDARY:
bin/$(PLAT)/$(PROFILE)/%.o$(SUFFIX): %.h
bin/$(PLAT)/$(PROFILE)/%.o$(SUFFIX): %.c 
	@$(ECHO) "# c source"
	@$(ECHO) "# Making $@ ..."
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $< $(INCLUDES)

CPP:
bin/$(PLAT)/$(PROFILE)/%.o$(SUFFIX): %.h
bin/$(PLAT)/$(PROFILE)/%.o$(SUFFIX): %.cpp
	@$(ECHO) "# cpp source"
	@$(ECHO) "# Making $@ ..."
	$(CXX) $(CPPFLAGS) $(CFLAGS) -o $@ -c $< $(INCLUDES)
	
	
#$(CXX) -c -o $*.o $(CFLAGS) $(INCLUDES) $<

$(CE_APPS_DIR)/rtcfg/osal/$(DEV)/lib/osal.a:
$(CE_APPS_DIR)/rtcfg/osal/$(DEV)/lib/cstubs.a:
	@echo "Making osal..."
	$(MAKE) -C $(CE_APPS_DIR)/rtcfg/osal \
		OSAL_INSTALL_DIR=$(OSAL_INSTALL_DIR) all

#
#  ======== clean ========
#
clean:
	$(RMDIR) bin/
	$(RM) $(PROGRAM).xv5T
#
#  @(#) ti.sdo.ce.examples.apps; 1, 0, 0,1; 2-24-2012 19:27:23; /db/atree/library/trees/ce/ce-t06/src/ xlibrary

#

