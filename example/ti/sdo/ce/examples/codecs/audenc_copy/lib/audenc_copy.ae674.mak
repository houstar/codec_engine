#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.elf.C674{1,0,7.3,4
#
ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.oe674.dep
endif

package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.oe674: | .interfaces
package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.oe674: package/package_ti.sdo.ce.examples.codecs.audenc_copy.c lib/audenc_copy.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/audenc_copy/package -fr=./package/lib/lib/audenc_copy/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/audenc_copy/package -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/audenc_copy/package -fr=./package/lib/lib/audenc_copy/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.oe674: export C_DIR=
package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.se674: | .interfaces
package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.se674: package/package_ti.sdo.ce.examples.codecs.audenc_copy.c lib/audenc_copy.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/audenc_copy/package -fr=./package/lib/lib/audenc_copy/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/audenc_copy/package -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/audenc_copy/package -fr=./package/lib/lib/audenc_copy/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.se674: export C_DIR=
package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/audenc_copy/audenc_copy.oe674.dep
endif

package/lib/lib/audenc_copy/audenc_copy.oe674: | .interfaces
package/lib/lib/audenc_copy/audenc_copy.oe674: audenc_copy.c lib/audenc_copy.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/audenc_copy -fr=./package/lib/lib/audenc_copy -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/audenc_copy -s oe674 $< -C   -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/audenc_copy -fr=./package/lib/lib/audenc_copy
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/audenc_copy/audenc_copy.oe674: export C_DIR=
package/lib/lib/audenc_copy/audenc_copy.oe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

package/lib/lib/audenc_copy/audenc_copy.se674: | .interfaces
package/lib/lib/audenc_copy/audenc_copy.se674: audenc_copy.c lib/audenc_copy.ae674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cle674 -n $< ...
	$(ti.targets.elf.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/audenc_copy -fr=./package/lib/lib/audenc_copy -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/audenc_copy -s oe674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 --abi=eabi -eo.oe674 -ea.se674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/elf/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.elf.C674.rootDir)/include -fs=./package/lib/lib/audenc_copy -fr=./package/lib/lib/audenc_copy
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/audenc_copy/audenc_copy.se674: export C_DIR=
package/lib/lib/audenc_copy/audenc_copy.se674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

clean,e674 ::
	-$(RM) package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.oe674
	-$(RM) package/lib/lib/audenc_copy/audenc_copy.oe674
	-$(RM) package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.se674
	-$(RM) package/lib/lib/audenc_copy/audenc_copy.se674

lib/audenc_copy.ae674: package/lib/lib/audenc_copy/package/package_ti.sdo.ce.examples.codecs.audenc_copy.oe674 package/lib/lib/audenc_copy/audenc_copy.oe674 lib/audenc_copy.ae674.mak

clean::
	-$(RM) lib/audenc_copy.ae674.mak
