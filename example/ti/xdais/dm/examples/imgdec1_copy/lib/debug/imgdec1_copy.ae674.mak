#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.elf.C674{1,0,7.3,4
#
ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.oe674.dep
endif

package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.oe674: | .interfaces
package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.oe674: package/package_ti.xdais.dm.examples.imgdec1_copy.c lib/debug/imgdec1_copy.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/debug/imgdec1_copy/package -fr=./package/lib/lib/debug/imgdec1_copy/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/debug/imgdec1_copy/package -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/debug/imgdec1_copy/package -fr=./package/lib/lib/debug/imgdec1_copy/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.oe674: export C_DIR=
package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.se674: | .interfaces
package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.se674: package/package_ti.xdais.dm.examples.imgdec1_copy.c lib/debug/imgdec1_copy.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/debug/imgdec1_copy/package -fr=./package/lib/lib/debug/imgdec1_copy/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/debug/imgdec1_copy/package -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/debug/imgdec1_copy/package -fr=./package/lib/lib/debug/imgdec1_copy/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.se674: export C_DIR=
package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/debug/imgdec1_copy/imgdec1_copy.oe674.dep
endif

package/lib/lib/debug/imgdec1_copy/imgdec1_copy.oe674: | .interfaces
package/lib/lib/debug/imgdec1_copy/imgdec1_copy.oe674: imgdec1_copy.c lib/debug/imgdec1_copy.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/debug/imgdec1_copy -fr=./package/lib/lib/debug/imgdec1_copy -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/debug/imgdec1_copy -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/debug/imgdec1_copy -fr=./package/lib/lib/debug/imgdec1_copy
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/debug/imgdec1_copy/imgdec1_copy.oe674: export C_DIR=
package/lib/lib/debug/imgdec1_copy/imgdec1_copy.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/debug/imgdec1_copy/imgdec1_copy.se674: | .interfaces
package/lib/lib/debug/imgdec1_copy/imgdec1_copy.se674: imgdec1_copy.c lib/debug/imgdec1_copy.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/debug/imgdec1_copy -fr=./package/lib/lib/debug/imgdec1_copy -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/debug/imgdec1_copy -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/debug/imgdec1_copy -fr=./package/lib/lib/debug/imgdec1_copy
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/debug/imgdec1_copy/imgdec1_copy.se674: export C_DIR=
package/lib/lib/debug/imgdec1_copy/imgdec1_copy.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

clean,e674 ::
	-$(RM) package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.oe674
	-$(RM) package/lib/lib/debug/imgdec1_copy/imgdec1_copy.oe674
	-$(RM) package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.se674
	-$(RM) package/lib/lib/debug/imgdec1_copy/imgdec1_copy.se674

lib/debug/imgdec1_copy.ae674: package/lib/lib/debug/imgdec1_copy/package/package_ti.xdais.dm.examples.imgdec1_copy.oe674 package/lib/lib/debug/imgdec1_copy/imgdec1_copy.oe674 lib/debug/imgdec1_copy.ae674.mak

clean::
	-$(RM) lib/debug/imgdec1_copy.ae674.mak
