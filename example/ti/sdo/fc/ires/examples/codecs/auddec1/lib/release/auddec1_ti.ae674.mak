#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.elf.C674{1,0,7.3,4
#
ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.oe674.dep
endif

package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.oe674: | .interfaces
package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.oe674: package/package_ti.sdo.fc.ires.examples.codecs.auddec1.c lib/release/auddec1_ti.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti/package -fr=./package/lib/lib/release/auddec1_ti/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/release/auddec1_ti/package -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti/package -fr=./package/lib/lib/release/auddec1_ti/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.oe674: export C_DIR=
package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.se674: | .interfaces
package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.se674: package/package_ti.sdo.fc.ires.examples.codecs.auddec1.c lib/release/auddec1_ti.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti/package -fr=./package/lib/lib/release/auddec1_ti/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/release/auddec1_ti/package -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti/package -fr=./package/lib/lib/release/auddec1_ti/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.se674: export C_DIR=
package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/release/auddec1_ti/auddec1_ti.oe674.dep
endif

package/lib/lib/release/auddec1_ti/auddec1_ti.oe674: | .interfaces
package/lib/lib/release/auddec1_ti/auddec1_ti.oe674: auddec1_ti.c lib/release/auddec1_ti.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti -fr=./package/lib/lib/release/auddec1_ti -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/release/auddec1_ti -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti -fr=./package/lib/lib/release/auddec1_ti
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/auddec1_ti.oe674: export C_DIR=
package/lib/lib/release/auddec1_ti/auddec1_ti.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/release/auddec1_ti/auddec1_ti.se674: | .interfaces
package/lib/lib/release/auddec1_ti/auddec1_ti.se674: auddec1_ti.c lib/release/auddec1_ti.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti -fr=./package/lib/lib/release/auddec1_ti -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/release/auddec1_ti -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti -fr=./package/lib/lib/release/auddec1_ti
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/auddec1_ti.se674: export C_DIR=
package/lib/lib/release/auddec1_ti/auddec1_ti.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/release/auddec1_ti/auddec1_ti_ires.oe674.dep
endif

package/lib/lib/release/auddec1_ti/auddec1_ti_ires.oe674: | .interfaces
package/lib/lib/release/auddec1_ti/auddec1_ti_ires.oe674: auddec1_ti_ires.c lib/release/auddec1_ti.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti -fr=./package/lib/lib/release/auddec1_ti -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/release/auddec1_ti -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti -fr=./package/lib/lib/release/auddec1_ti
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/auddec1_ti_ires.oe674: export C_DIR=
package/lib/lib/release/auddec1_ti/auddec1_ti_ires.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/release/auddec1_ti/auddec1_ti_ires.se674: | .interfaces
package/lib/lib/release/auddec1_ti/auddec1_ti_ires.se674: auddec1_ti_ires.c lib/release/auddec1_ti.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti -fr=./package/lib/lib/release/auddec1_ti -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/release/auddec1_ti -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/release/auddec1_ti -fr=./package/lib/lib/release/auddec1_ti
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/release/auddec1_ti/auddec1_ti_ires.se674: export C_DIR=
package/lib/lib/release/auddec1_ti/auddec1_ti_ires.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

clean,e674 ::
	-$(RM) package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.oe674
	-$(RM) package/lib/lib/release/auddec1_ti/auddec1_ti.oe674
	-$(RM) package/lib/lib/release/auddec1_ti/auddec1_ti_ires.oe674
	-$(RM) package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.se674
	-$(RM) package/lib/lib/release/auddec1_ti/auddec1_ti.se674
	-$(RM) package/lib/lib/release/auddec1_ti/auddec1_ti_ires.se674

lib/release/auddec1_ti.ae674: package/lib/lib/release/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.oe674 package/lib/lib/release/auddec1_ti/auddec1_ti.oe674 package/lib/lib/release/auddec1_ti/auddec1_ti_ires.oe674 lib/release/auddec1_ti.ae674.mak

clean::
	-$(RM) lib/release/auddec1_ti.ae674.mak
