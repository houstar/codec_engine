#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#

unexport MAKEFILE_LIST
MK_NOGENDEPS := $(filter clean,$(MAKECMDGOALS))
override PKGDIR = ti/xdais/dm/examples/imgenc1_copy
XDCINCS = -I. -I$(strip $(subst ;, -I,$(subst $(space),\$(space),$(XPKGPATH))))
XDCCFGDIR = package/cfg/

#
# The following dependencies ensure package.mak is rebuilt
# in the event that some included BOM script changes.
#
ifneq (clean,$(MAKECMDGOALS))
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/include/utils.tci:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/include/utils.tci
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/xdc.tci:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/xdc.tci
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/template.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/template.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/om2.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/om2.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/xmlgen.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/xmlgen.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/xmlgen2.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/xmlgen2.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/IPackage.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/IPackage.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/package.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/package.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/services/global/Clock.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/services/global/Clock.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/services/global/Trace.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/services/global/Trace.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/bld.js:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/bld.js
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/BuildEnvironment.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/BuildEnvironment.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/PackageContents.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/PackageContents.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/_gen.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/_gen.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Library.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Library.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Executable.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Executable.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Repository.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Repository.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Configuration.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Configuration.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Script.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Script.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Manifest.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Manifest.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Utils.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/Utils.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/ITarget.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/ITarget.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/ITarget2.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/ITarget2.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/ITargetFilter.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/ITargetFilter.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/package.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/bld/package.xs
/home/houstar/ezsdk/component-sources/codec_engine_3_22_01_06/codec_engine.bld:
package.mak: /home/houstar/ezsdk/component-sources/codec_engine_3_22_01_06/codec_engine.bld
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/ITarget.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/ITarget.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/C28_large.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/C28_large.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/C28_float.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/C28_float.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/package.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/package.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/elf/ITarget.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/elf/ITarget.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/elf/TMS470.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/elf/TMS470.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/elf/package.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/elf/package.xs
package.mak: package.bld
/home/houstar/ezsdk/component-sources/xdais_7_22_00_03/packages/ti/xdais/qualiti/QtiProject.xdt:
package.mak: /home/houstar/ezsdk/component-sources/xdais_7_22_00_03/packages/ti/xdais/qualiti/QtiProject.xdt
endif

ti.targets.C674.rootDir ?= /home/houstar/ezsdk/dsp-devkit/cgt6x_7_3_4
ti.targets.packageBase ?= /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/
ti.targets.elf.C674.rootDir ?= /home/houstar/ezsdk/dsp-devkit/cgt6x_7_3_4
ti.targets.elf.packageBase ?= /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/elf/
.PRECIOUS: $(XDCCFGDIR)/%.o674
.PHONY: all,674 .dlls,674 .executables,674 test,674
all,674: .executables,674
.executables,674: .libraries,674
.executables,674: .dlls,674
.dlls,674: .libraries,674
.libraries,674: .interfaces
	@$(RM) $@
	@$(TOUCH) "$@"

.help::
	@$(ECHO) xdc test,674
	@$(ECHO) xdc .executables,674
	@$(ECHO) xdc .libraries,674
	@$(ECHO) xdc .dlls,674

.PRECIOUS: $(XDCCFGDIR)/%.oe674
.PHONY: all,e674 .dlls,e674 .executables,e674 test,e674
all,e674: .executables,e674
.executables,e674: .libraries,e674
.executables,e674: .dlls,e674
.dlls,e674: .libraries,e674
.libraries,e674: .interfaces
	@$(RM) $@
	@$(TOUCH) "$@"

.help::
	@$(ECHO) xdc test,e674
	@$(ECHO) xdc .executables,e674
	@$(ECHO) xdc .libraries,e674
	@$(ECHO) xdc .dlls,e674


all: .executables 
.executables: .libraries .dlls
.libraries: .interfaces

PKGCFGS := $(wildcard package.xs) package/build.cfg
.interfaces: package/package.xdc.inc package/package.defs.h package.xdc $(PKGCFGS)

-include package/package.xdc.dep
package/%.xdc.inc package/%_ti.xdais.dm.examples.imgenc1_copy.c package/%.defs.h: %.xdc $(PKGCFGS)
	@$(MSG) generating interfaces for package ti.xdais.dm.examples.imgenc1_copy" (because $@ is older than $(firstword $?))" ...
	$(XSRUN) -f xdc/services/intern/cmd/build.xs $(MK_IDLOPTS) -m package/package.xdc.dep -i package/package.xdc.inc package.xdc

ifeq (,$(MK_NOGENDEPS))
-include package/package.cfg.dep
endif

package/package.ext.xml: package/package.cfg.xdc.inc
package/package.cfg.xdc.inc: $(XDCROOT)/packages/xdc/cfg/cfginc.js package.xdc
	@$(MSG) generating schema include file list ...
	$(CONFIG) -f $(XDCROOT)/packages/xdc/cfg/cfginc.js ti.xdais.dm.examples.imgenc1_copy $@

.libraries,674 .libraries: lib/debug/imgenc1_copy.a674

-include lib/debug/imgenc1_copy.a674.mak
lib/debug/imgenc1_copy.a674: 
	$(RM) $@
	@$(MSG) archiving package/lib/lib/debug/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.o674 package/lib/lib/debug/imgenc1_copy/imgenc1_copy.o674  into $@ ...
	$(ti.targets.C674.rootDir)/bin/ar6x  rq $@   package/lib/lib/debug/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.o674 package/lib/lib/debug/imgenc1_copy/imgenc1_copy.o674 
lib/debug/imgenc1_copy.a674: export C_DIR=
lib/debug/imgenc1_copy.a674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

clean,674 clean::
	-$(RM) lib/debug/imgenc1_copy.a674
.libraries,674 .libraries: lib/release/imgenc1_copy.a674

-include lib/release/imgenc1_copy.a674.mak
lib/release/imgenc1_copy.a674: 
	$(RM) $@
	@$(MSG) archiving package/lib/lib/release/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.o674 package/lib/lib/release/imgenc1_copy/imgenc1_copy.o674  into $@ ...
	$(ti.targets.C674.rootDir)/bin/ar6x  rq $@   package/lib/lib/release/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.o674 package/lib/lib/release/imgenc1_copy/imgenc1_copy.o674 
lib/release/imgenc1_copy.a674: export C_DIR=
lib/release/imgenc1_copy.a674: PATH:=$(ti.targets.C674.rootDir)/bin/:$(PATH)

clean,674 clean::
	-$(RM) lib/release/imgenc1_copy.a674
.libraries,e674 .libraries: lib/debug/imgenc1_copy.ae674

-include lib/debug/imgenc1_copy.ae674.mak
lib/debug/imgenc1_copy.ae674: 
	$(RM) $@
	@$(MSG) archiving package/lib/lib/debug/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.oe674 package/lib/lib/debug/imgenc1_copy/imgenc1_copy.oe674  into $@ ...
	$(ti.targets.elf.C674.rootDir)/bin/ar6x  rq $@   package/lib/lib/debug/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.oe674 package/lib/lib/debug/imgenc1_copy/imgenc1_copy.oe674 
lib/debug/imgenc1_copy.ae674: export C_DIR=
lib/debug/imgenc1_copy.ae674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

clean,e674 clean::
	-$(RM) lib/debug/imgenc1_copy.ae674
.libraries,e674 .libraries: lib/release/imgenc1_copy.ae674

-include lib/release/imgenc1_copy.ae674.mak
lib/release/imgenc1_copy.ae674: 
	$(RM) $@
	@$(MSG) archiving package/lib/lib/release/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.oe674 package/lib/lib/release/imgenc1_copy/imgenc1_copy.oe674  into $@ ...
	$(ti.targets.elf.C674.rootDir)/bin/ar6x  rq $@   package/lib/lib/release/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.oe674 package/lib/lib/release/imgenc1_copy/imgenc1_copy.oe674 
lib/release/imgenc1_copy.ae674: export C_DIR=
lib/release/imgenc1_copy.ae674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

clean,e674 clean::
	-$(RM) lib/release/imgenc1_copy.ae674
test:;
%,copy:
	@$(if $<,,$(MSG) don\'t know how to build $*; exit 1)
	@$(MSG) cp $< $@
	$(RM) $@
	$(CP) $< $@
.PHONY: package_ti.xdais.dm.examples.imgenc1_copy.o674,copy
package_ti.xdais.dm.examples.imgenc1_copy.o674,copy : package_ti.xdais.dm.examples.imgenc1_copy.o674,0,copy package_ti.xdais.dm.examples.imgenc1_copy.o674,1,copy 
	@

package_ti.xdais.dm.examples.imgenc1_copy.o674,0,copy : package/lib/lib/debug/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.o674
package_ti.xdais.dm.examples.imgenc1_copy.o674,1,copy : package/lib/lib/release/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.o674
.PHONY: package_ti.xdais.dm.examples.imgenc1_copy.s674,copy
package_ti.xdais.dm.examples.imgenc1_copy.s674,copy : package_ti.xdais.dm.examples.imgenc1_copy.s674,0,copy package_ti.xdais.dm.examples.imgenc1_copy.s674,1,copy 
	@

package_ti.xdais.dm.examples.imgenc1_copy.s674,0,copy : package/lib/lib/debug/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.s674
package_ti.xdais.dm.examples.imgenc1_copy.s674,1,copy : package/lib/lib/release/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.s674
.PHONY: imgenc1_copy.o674,copy
imgenc1_copy.o674,copy : imgenc1_copy.o674,0,copy imgenc1_copy.o674,1,copy 
	@

imgenc1_copy.o674,0,copy : package/lib/lib/debug/imgenc1_copy/imgenc1_copy.o674
imgenc1_copy.o674,1,copy : package/lib/lib/release/imgenc1_copy/imgenc1_copy.o674
.PHONY: imgenc1_copy.s674,copy
imgenc1_copy.s674,copy : imgenc1_copy.s674,0,copy imgenc1_copy.s674,1,copy 
	@

imgenc1_copy.s674,0,copy : package/lib/lib/debug/imgenc1_copy/imgenc1_copy.s674
imgenc1_copy.s674,1,copy : package/lib/lib/release/imgenc1_copy/imgenc1_copy.s674
.PHONY: package_ti.xdais.dm.examples.imgenc1_copy.oe674,copy
package_ti.xdais.dm.examples.imgenc1_copy.oe674,copy : package_ti.xdais.dm.examples.imgenc1_copy.oe674,0,copy package_ti.xdais.dm.examples.imgenc1_copy.oe674,1,copy 
	@

package_ti.xdais.dm.examples.imgenc1_copy.oe674,0,copy : package/lib/lib/debug/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.oe674
package_ti.xdais.dm.examples.imgenc1_copy.oe674,1,copy : package/lib/lib/release/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.oe674
.PHONY: package_ti.xdais.dm.examples.imgenc1_copy.se674,copy
package_ti.xdais.dm.examples.imgenc1_copy.se674,copy : package_ti.xdais.dm.examples.imgenc1_copy.se674,0,copy package_ti.xdais.dm.examples.imgenc1_copy.se674,1,copy 
	@

package_ti.xdais.dm.examples.imgenc1_copy.se674,0,copy : package/lib/lib/debug/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.se674
package_ti.xdais.dm.examples.imgenc1_copy.se674,1,copy : package/lib/lib/release/imgenc1_copy/package/package_ti.xdais.dm.examples.imgenc1_copy.se674
.PHONY: imgenc1_copy.oe674,copy
imgenc1_copy.oe674,copy : imgenc1_copy.oe674,0,copy imgenc1_copy.oe674,1,copy 
	@

imgenc1_copy.oe674,0,copy : package/lib/lib/debug/imgenc1_copy/imgenc1_copy.oe674
imgenc1_copy.oe674,1,copy : package/lib/lib/release/imgenc1_copy/imgenc1_copy.oe674
.PHONY: imgenc1_copy.se674,copy
imgenc1_copy.se674,copy : imgenc1_copy.se674,0,copy imgenc1_copy.se674,1,copy 
	@

imgenc1_copy.se674,0,copy : package/lib/lib/debug/imgenc1_copy/imgenc1_copy.se674
imgenc1_copy.se674,1,copy : package/lib/lib/release/imgenc1_copy/imgenc1_copy.se674

$(XDCCFGDIR)%.c $(XDCCFGDIR)%.h $(XDCCFGDIR)%.xdl: $(XDCCFGDIR)%.cfg .interfaces $(XDCROOT)/packages/xdc/cfg/Main.xs
	@$(MSG) "configuring $(_PROG_NAME) from $< ..."
	$(CONFIG) $(_PROG_XSOPTS) xdc.cfg $(_PROG_NAME) $(XDCCFGDIR)$*.cfg $(XDCCFGDIR)$*

.PHONY: release,ti_xdais_dm_examples_imgenc1_copy
ti_xdais_dm_examples_imgenc1_copy.tar: package/package.bld.xml
ti_xdais_dm_examples_imgenc1_copy.tar: package/package.ext.xml
ti_xdais_dm_examples_imgenc1_copy.tar: package/package.rel.dot
ti_xdais_dm_examples_imgenc1_copy.tar: package/build.cfg
ti_xdais_dm_examples_imgenc1_copy.tar: package/package.xdc.inc
ti_xdais_dm_examples_imgenc1_copy.tar: imgenc1_copy_ti.h
ti_xdais_dm_examples_imgenc1_copy.tar: pjt/imgenc1_copy.pjt
ti_xdais_dm_examples_imgenc1_copy.tar: package.bld
ti_xdais_dm_examples_imgenc1_copy.tar: imgenc1_copy_ti_priv.h
ti_xdais_dm_examples_imgenc1_copy.tar: lib/debug/imgenc1_copy.a674.qti
ti_xdais_dm_examples_imgenc1_copy.tar: lib/release/imgenc1_copy.a674.qti
ti_xdais_dm_examples_imgenc1_copy.tar: lib/debug/imgenc1_copy.ae674.qti
ti_xdais_dm_examples_imgenc1_copy.tar: lib/release/imgenc1_copy.ae674.qti
ti_xdais_dm_examples_imgenc1_copy.tar: package/package.cfg.xdc.inc
ti_xdais_dm_examples_imgenc1_copy.tar: lib/debug/imgenc1_copy.a674
ti_xdais_dm_examples_imgenc1_copy.tar: package/package_ti.xdais.dm.examples.imgenc1_copy.c
ti_xdais_dm_examples_imgenc1_copy.tar: imgenc1_copy.c
ti_xdais_dm_examples_imgenc1_copy.tar: lib/release/imgenc1_copy.a674
ti_xdais_dm_examples_imgenc1_copy.tar: lib/debug/imgenc1_copy.ae674
ti_xdais_dm_examples_imgenc1_copy.tar: lib/release/imgenc1_copy.ae674
ifeq (,$(MK_NOGENDEPS))
-include package/rel/ti_xdais_dm_examples_imgenc1_copy.tar.dep
endif
package/rel/ti_xdais_dm_examples_imgenc1_copy/ti/xdais/dm/examples/imgenc1_copy/package/package.rel.xml:

ti_xdais_dm_examples_imgenc1_copy.tar: package/rel/ti_xdais_dm_examples_imgenc1_copy.xdc.inc package/rel/ti_xdais_dm_examples_imgenc1_copy/ti/xdais/dm/examples/imgenc1_copy/package/package.rel.xml
	@$(MSG) making release file $@ "(because of $(firstword $?))" ...
	-$(RM) $@
	$(call MKRELTAR,package/rel/ti_xdais_dm_examples_imgenc1_copy.xdc.inc,package/rel/ti_xdais_dm_examples_imgenc1_copy.tar.dep)


release release,ti_xdais_dm_examples_imgenc1_copy: all ti_xdais_dm_examples_imgenc1_copy.tar
clean:: .clean
	-$(RM) ti_xdais_dm_examples_imgenc1_copy.tar
	-$(RM) package/rel/ti_xdais_dm_examples_imgenc1_copy.xdc.inc
	-$(RM) package/rel/ti_xdais_dm_examples_imgenc1_copy.tar.dep

clean:: .clean
	-$(RM) .libraries .libraries,*
clean:: 
	-$(RM) .dlls .dlls,*
#
# The following clean rule removes user specified
# generated files or directories.
#
	-$(RMDIR) lib/

ifneq (clean,$(MAKECMDGOALS))
ifeq (,$(wildcard lib))
    $(shell $(MKDIR) lib)
endif
ifeq (,$(wildcard lib/debug))
    $(shell $(MKDIR) lib/debug)
endif
ifeq (,$(wildcard lib/release))
    $(shell $(MKDIR) lib/release)
endif
ifeq (,$(wildcard package))
    $(shell $(MKDIR) package)
endif
ifeq (,$(wildcard package/cfg))
    $(shell $(MKDIR) package/cfg)
endif
ifeq (,$(wildcard package/lib))
    $(shell $(MKDIR) package/lib)
endif
ifeq (,$(wildcard package/rel))
    $(shell $(MKDIR) package/rel)
endif
ifeq (,$(wildcard package/internal))
    $(shell $(MKDIR) package/internal)
endif
ifeq (,$(wildcard package/external))
    $(shell $(MKDIR) package/external)
endif
ifeq (,$(wildcard package/lib/lib))
    $(shell $(MKDIR) package/lib/lib)
endif
ifeq (,$(wildcard package/lib/lib/debug))
    $(shell $(MKDIR) package/lib/lib/debug)
endif
ifeq (,$(wildcard package/lib/lib/debug/imgenc1_copy))
    $(shell $(MKDIR) package/lib/lib/debug/imgenc1_copy)
endif
ifeq (,$(wildcard package/lib/lib/debug/imgenc1_copy/package))
    $(shell $(MKDIR) package/lib/lib/debug/imgenc1_copy/package)
endif
ifeq (,$(wildcard package/lib/lib/release))
    $(shell $(MKDIR) package/lib/lib/release)
endif
ifeq (,$(wildcard package/lib/lib/release/imgenc1_copy))
    $(shell $(MKDIR) package/lib/lib/release/imgenc1_copy)
endif
ifeq (,$(wildcard package/lib/lib/release/imgenc1_copy/package))
    $(shell $(MKDIR) package/lib/lib/release/imgenc1_copy/package)
endif
endif
clean::
	-$(RMDIR) package


clean:: 
	-$(RM) package/ti.xdais.dm.examples.imgenc1_copy.pjt
