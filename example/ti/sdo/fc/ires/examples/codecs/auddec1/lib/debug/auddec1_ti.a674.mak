#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.C674{1,0,7.3,4
#
ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.o674.dep
endif

package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.o674: | .interfaces
package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.o674: package/package_ti.sdo.fc.ires.examples.codecs.auddec1.c lib/debug/auddec1_ti.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti/package -fr=./package/lib/lib/debug/auddec1_ti/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/debug/auddec1_ti/package -s o674 $< -C   -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti/package -fr=./package/lib/lib/debug/auddec1_ti/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.o674: export C_DIR=
package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.o674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.s674: | .interfaces
package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.s674: package/package_ti.sdo.fc.ires.examples.codecs.auddec1.c lib/debug/auddec1_ti.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 -n $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti/package -fr=./package/lib/lib/debug/auddec1_ti/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/debug/auddec1_ti/package -s o674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti/package -fr=./package/lib/lib/debug/auddec1_ti/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.s674: export C_DIR=
package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.s674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/debug/auddec1_ti/auddec1_ti.o674.dep
endif

package/lib/lib/debug/auddec1_ti/auddec1_ti.o674: | .interfaces
package/lib/lib/debug/auddec1_ti/auddec1_ti.o674: auddec1_ti.c lib/debug/auddec1_ti.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti -fr=./package/lib/lib/debug/auddec1_ti -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/debug/auddec1_ti -s o674 $< -C   -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti -fr=./package/lib/lib/debug/auddec1_ti
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/debug/auddec1_ti/auddec1_ti.o674: export C_DIR=
package/lib/lib/debug/auddec1_ti/auddec1_ti.o674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

package/lib/lib/debug/auddec1_ti/auddec1_ti.s674: | .interfaces
package/lib/lib/debug/auddec1_ti/auddec1_ti.s674: auddec1_ti.c lib/debug/auddec1_ti.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 -n $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti -fr=./package/lib/lib/debug/auddec1_ti -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/debug/auddec1_ti -s o674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti -fr=./package/lib/lib/debug/auddec1_ti
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/debug/auddec1_ti/auddec1_ti.s674: export C_DIR=
package/lib/lib/debug/auddec1_ti/auddec1_ti.s674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.o674.dep
endif

package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.o674: | .interfaces
package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.o674: auddec1_ti_ires.c lib/debug/auddec1_ti.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti -fr=./package/lib/lib/debug/auddec1_ti -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/debug/auddec1_ti -s o674 $< -C   -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti -fr=./package/lib/lib/debug/auddec1_ti
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.o674: export C_DIR=
package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.o674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.s674: | .interfaces
package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.s674: auddec1_ti_ires.c lib/debug/auddec1_ti.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 -n $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti -fr=./package/lib/lib/debug/auddec1_ti -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/debug/auddec1_ti -s o674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -D_DEBUG_=1 -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_debug -Dxdc_bld__vers_1_0_7_3_4 --symdebug:dwarf  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/debug/auddec1_ti -fr=./package/lib/lib/debug/auddec1_ti
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.s674: export C_DIR=
package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.s674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

clean,674 ::
	-$(RM) package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.o674
	-$(RM) package/lib/lib/debug/auddec1_ti/auddec1_ti.o674
	-$(RM) package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.o674
	-$(RM) package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.s674
	-$(RM) package/lib/lib/debug/auddec1_ti/auddec1_ti.s674
	-$(RM) package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.s674

lib/debug/auddec1_ti.a674: package/lib/lib/debug/auddec1_ti/package/package_ti.sdo.fc.ires.examples.codecs.auddec1.o674 package/lib/lib/debug/auddec1_ti/auddec1_ti.o674 package/lib/lib/debug/auddec1_ti/auddec1_ti_ires.o674 lib/debug/auddec1_ti.a674.mak

clean::
	-$(RM) lib/debug/auddec1_ti.a674.mak