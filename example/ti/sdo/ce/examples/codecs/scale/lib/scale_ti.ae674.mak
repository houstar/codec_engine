#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.elf.C674{1,0,7.3,4
#
ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.oe674.dep
endif

package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.oe674: | .interfaces
package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.oe674: package/package_ti.sdo.ce.examples.codecs.scale.c lib/scale_ti.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/scale_ti/package -fr=./package/lib/lib/scale_ti/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_ti/package -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/scale_ti/package -fr=./package/lib/lib/scale_ti/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.oe674: export C_DIR=
package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.se674: | .interfaces
package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.se674: package/package_ti.sdo.ce.examples.codecs.scale.c lib/scale_ti.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/scale_ti/package -fr=./package/lib/lib/scale_ti/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_ti/package -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/scale_ti/package -fr=./package/lib/lib/scale_ti/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.se674: export C_DIR=
package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/scale_ti/scale_ti_impl.oe674.dep
endif

package/lib/lib/scale_ti/scale_ti_impl.oe674: | .interfaces
package/lib/lib/scale_ti/scale_ti_impl.oe674: scale_ti_impl.c lib/scale_ti.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/scale_ti -fr=./package/lib/lib/scale_ti -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_ti -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/scale_ti -fr=./package/lib/lib/scale_ti
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_ti/scale_ti_impl.oe674: export C_DIR=
package/lib/lib/scale_ti/scale_ti_impl.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/scale_ti/scale_ti_impl.se674: | .interfaces
package/lib/lib/scale_ti/scale_ti_impl.se674: scale_ti_impl.c lib/scale_ti.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/scale_ti -fr=./package/lib/lib/scale_ti -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale_ti -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/scale_ti -fr=./package/lib/lib/scale_ti
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale_ti/scale_ti_impl.se674: export C_DIR=
package/lib/lib/scale_ti/scale_ti_impl.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

clean,e674 ::
	-$(RM) package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.oe674
	-$(RM) package/lib/lib/scale_ti/scale_ti_impl.oe674
	-$(RM) package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.se674
	-$(RM) package/lib/lib/scale_ti/scale_ti_impl.se674

lib/scale_ti.ae674: package/lib/lib/scale_ti/package/package_ti.sdo.ce.examples.codecs.scale.oe674 package/lib/lib/scale_ti/scale_ti_impl.oe674 lib/scale_ti.ae674.mak

clean::
	-$(RM) lib/scale_ti.ae674.mak