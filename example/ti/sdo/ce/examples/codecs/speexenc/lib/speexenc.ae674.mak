#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.elf.C674{1,0,7.3,4
#
ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.oe674.dep
endif

package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.oe674: | .interfaces
package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.oe674: package/package_ti.sdo.ce.examples.codecs.speexenc.c lib/speexenc.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc/package -fr=./package/lib/lib/speexenc/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/speexenc/package -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc/package -fr=./package/lib/lib/speexenc/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.oe674: export C_DIR=
package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.se674: | .interfaces
package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.se674: package/package_ti.sdo.ce.examples.codecs.speexenc.c lib/speexenc.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc/package -fr=./package/lib/lib/speexenc/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/speexenc/package -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc/package -fr=./package/lib/lib/speexenc/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.se674: export C_DIR=
package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/speexenc/speexenc_copy.oe674.dep
endif

package/lib/lib/speexenc/speexenc_copy.oe674: | .interfaces
package/lib/lib/speexenc/speexenc_copy.oe674: speexenc_copy.c lib/speexenc.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/speexenc -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/speexenc/speexenc_copy.oe674: export C_DIR=
package/lib/lib/speexenc/speexenc_copy.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/speexenc/speexenc_copy.se674: | .interfaces
package/lib/lib/speexenc/speexenc_copy.se674: speexenc_copy.c lib/speexenc.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/speexenc -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/speexenc/speexenc_copy.se674: export C_DIR=
package/lib/lib/speexenc/speexenc_copy.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/speexenc/SpeexCod.oe674.dep
endif

package/lib/lib/speexenc/SpeexCod.oe674: | .interfaces
package/lib/lib/speexenc/SpeexCod.oe674: SpeexCod.c lib/speexenc.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/speexenc -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/speexenc/SpeexCod.oe674: export C_DIR=
package/lib/lib/speexenc/SpeexCod.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/speexenc/SpeexCod.se674: | .interfaces
package/lib/lib/speexenc/SpeexCod.se674: SpeexCod.c lib/speexenc.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/speexenc -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/speexenc/SpeexCod.se674: export C_DIR=
package/lib/lib/speexenc/SpeexCod.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/speexenc/mixer.oe674.dep
endif

package/lib/lib/speexenc/mixer.oe674: | .interfaces
package/lib/lib/speexenc/mixer.oe674: mixer.c lib/speexenc.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/speexenc -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/speexenc/mixer.oe674: export C_DIR=
package/lib/lib/speexenc/mixer.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/speexenc/mixer.se674: | .interfaces
package/lib/lib/speexenc/mixer.se674: mixer.c lib/speexenc.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/speexenc -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/speexenc/mixer.se674: export C_DIR=
package/lib/lib/speexenc/mixer.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/speexenc/mix_linear2ulaw_table.oe674.dep
endif

package/lib/lib/speexenc/mix_linear2ulaw_table.oe674: | .interfaces
package/lib/lib/speexenc/mix_linear2ulaw_table.oe674: mix_linear2ulaw_table.c lib/speexenc.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/speexenc -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/speexenc/mix_linear2ulaw_table.oe674: export C_DIR=
package/lib/lib/speexenc/mix_linear2ulaw_table.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/speexenc/mix_linear2ulaw_table.se674: | .interfaces
package/lib/lib/speexenc/mix_linear2ulaw_table.se674: mix_linear2ulaw_table.c lib/speexenc.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/speexenc -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/speexenc -fr=./package/lib/lib/speexenc
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/speexenc/mix_linear2ulaw_table.se674: export C_DIR=
package/lib/lib/speexenc/mix_linear2ulaw_table.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

clean,e674 ::
	-$(RM) package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.oe674
	-$(RM) package/lib/lib/speexenc/speexenc_copy.oe674
	-$(RM) package/lib/lib/speexenc/SpeexCod.oe674
	-$(RM) package/lib/lib/speexenc/mixer.oe674
	-$(RM) package/lib/lib/speexenc/mix_linear2ulaw_table.oe674
	-$(RM) package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.se674
	-$(RM) package/lib/lib/speexenc/speexenc_copy.se674
	-$(RM) package/lib/lib/speexenc/SpeexCod.se674
	-$(RM) package/lib/lib/speexenc/mixer.se674
	-$(RM) package/lib/lib/speexenc/mix_linear2ulaw_table.se674

lib/speexenc.ae674: package/lib/lib/speexenc/package/package_ti.sdo.ce.examples.codecs.speexenc.oe674 package/lib/lib/speexenc/speexenc_copy.oe674 package/lib/lib/speexenc/SpeexCod.oe674 package/lib/lib/speexenc/mixer.oe674 package/lib/lib/speexenc/mix_linear2ulaw_table.oe674 lib/speexenc.ae674.mak

clean::
	-$(RM) lib/speexenc.ae674.mak
