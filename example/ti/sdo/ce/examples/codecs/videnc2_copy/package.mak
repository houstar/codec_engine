#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#

unexport MAKEFILE_LIST
MK_NOGENDEPS := $(filter clean,$(MAKECMDGOALS))
override PKGDIR = ti/sdo/ce/examples/codecs/videnc2_copy
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
endif

all,674 .libraries,674 .dlls,674 .executables,674 test,674:;
all,e674 .libraries,e674 .dlls,e674 .executables,e674 test,e674:;

all: .executables 
.executables: .libraries .dlls
.libraries: .interfaces

PKGCFGS := $(wildcard package.xs) package/build.cfg
.interfaces: package/package.xdc.inc package/package.defs.h package.xdc $(PKGCFGS)

-include package/package.xdc.dep
package/%.xdc.inc package/%_ti.sdo.ce.examples.codecs.videnc2_copy.c package/%.defs.h: %.xdc $(PKGCFGS)
	@$(MSG) generating interfaces for package ti.sdo.ce.examples.codecs.videnc2_copy" (because $@ is older than $(firstword $?))" ...
	$(XSRUN) -f xdc/services/intern/cmd/build.xs $(MK_IDLOPTS) -m package/package.xdc.dep -i package/package.xdc.inc package.xdc

ifeq (,$(MK_NOGENDEPS))
-include package/package.cfg.dep
endif

package/package.ext.xml: package/package.cfg.xdc.inc
package/package.cfg.xdc.inc: $(XDCROOT)/packages/xdc/cfg/cfginc.js package.xdc
	@$(MSG) generating schema include file list ...
	$(CONFIG) -f $(XDCROOT)/packages/xdc/cfg/cfginc.js ti.sdo.ce.examples.codecs.videnc2_copy $@

test:;
%,copy:
	@$(if $<,,$(MSG) don\'t know how to build $*; exit 1)
	@$(MSG) cp $< $@
	$(RM) $@
	$(CP) $< $@

$(XDCCFGDIR)%.c $(XDCCFGDIR)%.h $(XDCCFGDIR)%.xdl: $(XDCCFGDIR)%.cfg .interfaces $(XDCROOT)/packages/xdc/cfg/Main.xs
	@$(MSG) "configuring $(_PROG_NAME) from $< ..."
	$(CONFIG) $(_PROG_XSOPTS) xdc.cfg $(_PROG_NAME) $(XDCCFGDIR)$*.cfg $(XDCCFGDIR)$*

.PHONY: release,ti_sdo_ce_examples_codecs_videnc2_copy
ti_sdo_ce_examples_codecs_videnc2_copy.tar: package/package.bld.xml
ti_sdo_ce_examples_codecs_videnc2_copy.tar: package/package.ext.xml
ti_sdo_ce_examples_codecs_videnc2_copy.tar: package/package.rel.dot
ti_sdo_ce_examples_codecs_videnc2_copy.tar: package/build.cfg
ti_sdo_ce_examples_codecs_videnc2_copy.tar: package/package.xdc.inc
ti_sdo_ce_examples_codecs_videnc2_copy.tar: package/package.cfg.xdc.inc
ifeq (,$(MK_NOGENDEPS))
-include package/rel/ti_sdo_ce_examples_codecs_videnc2_copy.tar.dep
endif
package/rel/ti_sdo_ce_examples_codecs_videnc2_copy/ti/sdo/ce/examples/codecs/videnc2_copy/package/package.rel.xml:

ti_sdo_ce_examples_codecs_videnc2_copy.tar: package/rel/ti_sdo_ce_examples_codecs_videnc2_copy.xdc.inc package/rel/ti_sdo_ce_examples_codecs_videnc2_copy/ti/sdo/ce/examples/codecs/videnc2_copy/package/package.rel.xml
	@$(MSG) making release file $@ "(because of $(firstword $?))" ...
	-$(RM) $@
	$(call MKRELTAR,package/rel/ti_sdo_ce_examples_codecs_videnc2_copy.xdc.inc,package/rel/ti_sdo_ce_examples_codecs_videnc2_copy.tar.dep)


release release,ti_sdo_ce_examples_codecs_videnc2_copy: all ti_sdo_ce_examples_codecs_videnc2_copy.tar
clean:: .clean
	-$(RM) ti_sdo_ce_examples_codecs_videnc2_copy.tar
	-$(RM) package/rel/ti_sdo_ce_examples_codecs_videnc2_copy.xdc.inc
	-$(RM) package/rel/ti_sdo_ce_examples_codecs_videnc2_copy.tar.dep

clean:: .clean
	-$(RM) .libraries .libraries,*
clean:: 
	-$(RM) .dlls .dlls,*
#
# The following clean rule removes user specified
# generated files or directories.
#

ifneq (clean,$(MAKECMDGOALS))
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
endif
clean::
	-$(RMDIR) package


