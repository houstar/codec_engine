#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#
#  target compatibility key = ti.targets.C674{1,0,7.3,4
#
ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.o674.dep
endif

package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.o674: | .interfaces
package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.o674: package/package_ti.sdo.ce.examples.extensions.scale.c lib/scale.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale/package -fr=./package/lib/lib/scale/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale/package -s o674 $< -C   -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale/package -fr=./package/lib/lib/scale/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.o674: export C_DIR=
package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.o674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.s674: | .interfaces
package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.s674: package/package_ti.sdo.ce.examples.extensions.scale.c lib/scale.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 -n $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale/package -fr=./package/lib/lib/scale/package -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale/package -s o674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale/package -fr=./package/lib/lib/scale/package
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.s674: export C_DIR=
package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.s674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/scale/scale.o674.dep
endif

package/lib/lib/scale/scale.o674: | .interfaces
package/lib/lib/scale/scale.o674: scale.c lib/scale.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale -s o674 $< -C   -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale.o674: export C_DIR=
package/lib/lib/scale/scale.o674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

package/lib/lib/scale/scale.s674: | .interfaces
package/lib/lib/scale/scale.s674: scale.c lib/scale.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 -n $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale -s o674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale.s674: export C_DIR=
package/lib/lib/scale/scale.s674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/scale/scale_skel.o674.dep
endif

package/lib/lib/scale/scale_skel.o674: | .interfaces
package/lib/lib/scale/scale_skel.o674: scale_skel.c lib/scale.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale -s o674 $< -C   -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale_skel.o674: export C_DIR=
package/lib/lib/scale/scale_skel.o674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

package/lib/lib/scale/scale_skel.s674: | .interfaces
package/lib/lib/scale/scale_skel.s674: scale_skel.c lib/scale.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 -n $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale -s o674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale_skel.s674: export C_DIR=
package/lib/lib/scale/scale_skel.s674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

ifeq (,$(MK_NOGENDEPS))
-include package/lib/lib/scale/scale_stubs.o674.dep
endif

package/lib/lib/scale/scale_stubs.o674: | .interfaces
package/lib/lib/scale/scale_stubs.o674: scale_stubs.c lib/scale.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c  -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale -s o674 $< -C   -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale_stubs.o674: export C_DIR=
package/lib/lib/scale/scale_stubs.o674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

package/lib/lib/scale/scale_stubs.s674: | .interfaces
package/lib/lib/scale/scale_stubs.s674: scale_stubs.c lib/scale.a674.mak
	@$(RM) $@.dep
	$(RM) $@
	@$(MSG) cl674 -n $< ...
	$(ti.targets.C674.rootDir)/bin/cl6x -c -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale -fc $<
	$(MKDEP) -a $@.dep -p package/lib/lib/scale -s o674 $< -C  -n -s --symdebug:none -qq -pdsw225 -mv6740 -eo.o674 -ea.s674  -pdr -pden -pds=880 -pds=238 -pds452 -pds195 -mi10 -mo -g  -Dxdc_target_name__=C674 -Dxdc_target_types__=ti/targets/std.h -Dxdc_bld__profile_release -Dxdc_bld__vers_1_0_7_3_4 -O2  $(XDCINCS) -I$(ti.targets.C674.rootDir)/include -fs=./package/lib/lib/scale -fr=./package/lib/lib/scale
	-@$(FIXDEP) $@.dep $@.dep
	
package/lib/lib/scale/scale_stubs.s674: export C_DIR=
package/lib/lib/scale/scale_stubs.s674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

clean,674 ::
	-$(RM) package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.o674
	-$(RM) package/lib/lib/scale/scale.o674
	-$(RM) package/lib/lib/scale/scale_skel.o674
	-$(RM) package/lib/lib/scale/scale_stubs.o674
	-$(RM) package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.s674
	-$(RM) package/lib/lib/scale/scale.s674
	-$(RM) package/lib/lib/scale/scale_skel.s674
	-$(RM) package/lib/lib/scale/scale_stubs.s674

lib/scale.a674: package/lib/lib/scale/package/package_ti.sdo.ce.examples.extensions.scale.o674 package/lib/lib/scale/scale.o674 package/lib/lib/scale/scale_skel.o674 package/lib/lib/scale/scale_stubs.o674 lib/scale.a674.mak

clean::
	-$(RM) lib/scale.a674.mak