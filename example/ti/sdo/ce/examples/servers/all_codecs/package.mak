#
#  Do not edit this file.  This file is generated from 
#  package.bld.  Any modifications to this file will be 
#  overwritten whenever makefiles are re-generated.
#

unexport MAKEFILE_LIST
MK_NOGENDEPS := $(filter clean,$(MAKECMDGOALS))
override PKGDIR = ti/sdo/ce/examples/servers/all_codecs
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
package.mak: _config.bld
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
package.mak: serverplatforms.xs
package.mak: package.bld
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/platform/ExeContext.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/platform/ExeContext.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/platform/Utils.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/platform/Utils.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/platform/IPlatform.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/platform/IPlatform.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/platform/IPeripheral.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/platform/IPeripheral.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/platform/package.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/xdc/platform/package.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320C6452.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320C6452.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320CDM647.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320CDM647.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320CDM648.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320CDM648.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320CTNETV2685.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320CTNETV2685.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320C6428.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320C6428.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C6x1x.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C6x1x.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/c6x.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/c6x.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C64_128K.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C64_128K.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C64_256K.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C64_256K.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C64_1M.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C64_1M.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C64_512K.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C64_512K.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320C6421.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320C6421.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C642x.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C642x.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/IAntara.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/IAntara.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CDRA44x.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CDRA44x.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CDRA45x.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CDRA45x.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/IDaVinci.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/IDaVinci.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320CDM6431.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320CDM6431.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CDM6467.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CDM6467.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/IHimalaya.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/IHimalaya.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CTCI6484.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CTCI6484.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CTCI6486.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CTCI6486.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CTCI648x.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CTCI648x.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CTCI6497.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CTCI6497.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320TCI6608.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320TCI6608.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320TCI6616.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320TCI6616.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/Kepler.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/Kepler.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320C6454.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320C6454.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/IOMAP2x3x.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/IOMAP2x3x.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/IOMAP3xxx.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/IOMAP3xxx.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C6x0x.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320C6x0x.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CDRx40x.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320CDRx40x.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320CF761990.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TMS320CF761990.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/OMAP4430.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/OMAP4430.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/OMAP5430.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/OMAP5430.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320DA8xx.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITMS320DA8xx.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TNETV107X.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/TNETV107X.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITI8168.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITI8168.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITI8148.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITI8148.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITI811X.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/ITI811X.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/Arctic.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/Arctic.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/Vayu.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/Vayu.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/package.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/c6000/package.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/platforms/evmDA830/Platform.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/platforms/evmDA830/Platform.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/OMAP4430.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/OMAP4430.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ILM3Sx7xx.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ILM3Sx7xx.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ILM3SxBxx.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ILM3SxBxx.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ILM3Sx9xx.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ILM3Sx9xx.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ITI8168.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ITI8168.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ITI8148.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ITI8148.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/F28M35x.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/F28M35x.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/Stellaris.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/Stellaris.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/CortexM3.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/CortexM3.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ITI813X.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ITI813X.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ITI811X.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexm3/ITI811X.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/IOMAP3xxx.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/IOMAP3xxx.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/ITI8168.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/ITI8168.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/ITI8148.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/ITI8148.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/IAM335X.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/IAM335X.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/ITI813X.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/ITI813X.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/ITI811X.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/catalog/arm/cortexa8/ITI811X.xs
/home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/platforms/evmTI816X/Platform.xs:
package.mak: /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/platforms/evmTI816X/Platform.xs
endif

ti.targets.elf.C674.rootDir ?= /home/houstar/ezsdk/dsp-devkit/cgt6x_7_3_4
ti.targets.elf.packageBase ?= /home/houstar/ezsdk/component-sources/xdctools_3_23_03_53/packages/ti/targets/elf/
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

all,674 .libraries,674 .dlls,674 .executables,674 test,674:;

all: .executables 
.executables: .libraries .dlls
.libraries: .interfaces

PKGCFGS := $(wildcard package.xs) package/build.cfg
.interfaces: package/package.xdc.inc package/package.defs.h package.xdc $(PKGCFGS)

-include package/package.xdc.dep
package/%.xdc.inc package/%_ti.sdo.ce.examples.servers.all_codecs.c package/%.defs.h: %.xdc $(PKGCFGS)
	@$(MSG) generating interfaces for package ti.sdo.ce.examples.servers.all_codecs" (because $@ is older than $(firstword $?))" ...
	$(XSRUN) -f xdc/services/intern/cmd/build.xs $(MK_IDLOPTS) -m package/package.xdc.dep -i package/package.xdc.inc package.xdc

.dlls,e674 .dlls: bin/ti_platforms_evmTI816X/all_DSP.pe674

-include package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.mak
-include package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.cfg.mak
ifeq (,$(MK_NOGENDEPS))
-include package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.dep
endif
bin/ti_platforms_evmTI816X/all_DSP.pe674: package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.xdl
	@


ifeq (,$(wildcard .libraries,e674))
bin/ti_platforms_evmTI816X/all_DSP.pe674 package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.c: .libraries,e674
endif

package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.c package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.h package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.xdl: override _PROG_NAME := bin/ti_platforms_evmTI816X/all_DSP.xe674
package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.c: package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.cfg

clean:: clean,e674
	-$(RM) package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.cfg
	-$(RM) package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.dep
	-$(RM) package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.c
	-$(RM) package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.xdc.inc

clean,e674::
	-$(RM) bin/ti_platforms_evmTI816X/all_DSP.pe674
.executables,e674 .executables: bin/ti_platforms_evmTI816X/all_DSP.xe674

bin/ti_platforms_evmTI816X/all_DSP.xe674: |bin/ti_platforms_evmTI816X/all_DSP.pe674

-include package/cfg/bin/ti_platforms_evmTI816X/all_DSP.xe674.mak
bin/ti_platforms_evmTI816X/all_DSP.xe674: package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.oe674 
	$(RM) $@
	@$(MSG) lnke674 $@ ...
	$(RM) $(XDCCFGDIR)/$@.map
	$(ti.targets.elf.C674.rootDir)/bin/lnk6x -w -q -u _c_int00 -fs $(XDCCFGDIR)$(dir $@) -l link.cmd -q -o $@ package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.oe674  package/cfg/bin/ti_platforms_evmTI816X/all_DSP/main.oe674  package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.xdl --abi=eabi -c -m $(XDCCFGDIR)/$@.map -l $(ti.targets.elf.C674.rootDir)/lib/rts6740_elf.lib --stack=0x80000 --compress_dwarf -l ../../codecs/speex/libspeex.lib -l ../../codecs/ec/avdAEC.lib -l ../../codecs/aac/libaac.lib -l ${EZSDK}/dsp-devkit/cgt6x_7_3_4/lib/libc.a
	
bin/ti_platforms_evmTI816X/all_DSP.xe674: export C_DIR=
bin/ti_platforms_evmTI816X/all_DSP.xe674: PATH:=$(ti.targets.elf.C674.rootDir)/bin/:$(PATH)

bin/ti_platforms_evmTI816X/all_DSP.test test,e674 test: bin/ti_platforms_evmTI816X/all_DSP.xe674.test

bin/ti_platforms_evmTI816X/all_DSP.xe674.test:: bin/ti_platforms_evmTI816X/all_DSP.xe674
ifeq (,$(_TESTLEVEL))
	@$(MAKE) -R -r --no-print-directory -f $(XDCROOT)/packages/xdc/bld/xdc.mak _TESTLEVEL=1 bin/ti_platforms_evmTI816X/all_DSP.xe674.test
else
	@$(MSG) running $<  ...
	$(call EXEC.bin/ti_platforms_evmTI816X/all_DSP.xe674, ) 
endif

clean,e674::
	-$(RM) bin/ti_platforms_evmTI816X/.tmp,all_DSP.xe674,0,*


clean:: clean,e674

clean,e674::
	-$(RM) bin/ti_platforms_evmTI816X/all_DSP.xe674
clean:: 
	-$(RM) package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.pjt
%,copy:
	@$(if $<,,$(MSG) don\'t know how to build $*; exit 1)
	@$(MSG) cp $< $@
	$(RM) $@
	$(CP) $< $@
all_DSP_pe674.oe674,copy : package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.oe674
all_DSP_pe674.se674,copy : package/cfg/bin/ti_platforms_evmTI816X/all_DSP_pe674.se674
main.oe674,copy : package/cfg/bin/ti_platforms_evmTI816X/all_DSP/main.oe674
main.se674,copy : package/cfg/bin/ti_platforms_evmTI816X/all_DSP/main.se674

$(XDCCFGDIR)%.c $(XDCCFGDIR)%.h $(XDCCFGDIR)%.xdl: $(XDCCFGDIR)%.cfg .interfaces $(XDCROOT)/packages/xdc/cfg/Main.xs
	@$(MSG) "configuring $(_PROG_NAME) from $< ..."
	$(CONFIG) $(_PROG_XSOPTS) xdc.cfg $(_PROG_NAME) $(XDCCFGDIR)$*.cfg $(XDCCFGDIR)$*

.PHONY: release,ti_sdo_ce_examples_servers_all_codecs
ti_sdo_ce_examples_servers_all_codecs.tar: package/package.bld.xml
ti_sdo_ce_examples_servers_all_codecs.tar: package/package.ext.xml
ti_sdo_ce_examples_servers_all_codecs.tar: package/package.rel.dot
ti_sdo_ce_examples_servers_all_codecs.tar: package/build.cfg
ti_sdo_ce_examples_servers_all_codecs.tar: package/package.xdc.inc
ti_sdo_ce_examples_servers_all_codecs.tar: main.c
ti_sdo_ce_examples_servers_all_codecs.tar: setid.c
ti_sdo_ce_examples_servers_all_codecs.tar: all_mc.cfg
ti_sdo_ce_examples_servers_all_codecs.tar: all_syslink.cfg
ti_sdo_ce_examples_servers_all_codecs.tar: heaps.cfg
ti_sdo_ce_examples_servers_all_codecs.tar: link.cmd
ti_sdo_ce_examples_servers_all_codecs.tar: package.bld
ti_sdo_ce_examples_servers_all_codecs.tar: _config.bld
ti_sdo_ce_examples_servers_all_codecs.tar: makefile
ti_sdo_ce_examples_servers_all_codecs.tar: ti_platforms_evm3530.cfg
ti_sdo_ce_examples_servers_all_codecs.tar: ti_platforms_evm6472.cfg
ti_sdo_ce_examples_servers_all_codecs.tar: ti_platforms_evmTI814X.cfg
ti_sdo_ce_examples_servers_all_codecs.tar: ti_platforms_evmTI816X.cfg
ti_sdo_ce_examples_servers_all_codecs.tar: ti_platforms_evmTI813X.cfg
ti_sdo_ce_examples_servers_all_codecs.tar: serverplatforms.xs
ti_sdo_ce_examples_servers_all_codecs.tar: package/info
ti_sdo_ce_examples_servers_all_codecs.tar: android_config.bld
ifeq (,$(MK_NOGENDEPS))
-include package/rel/ti_sdo_ce_examples_servers_all_codecs.tar.dep
endif
package/rel/ti_sdo_ce_examples_servers_all_codecs/ti/sdo/ce/examples/servers/all_codecs/package/package.rel.xml:

ti_sdo_ce_examples_servers_all_codecs.tar: package/rel/ti_sdo_ce_examples_servers_all_codecs.xdc.inc package/rel/ti_sdo_ce_examples_servers_all_codecs/ti/sdo/ce/examples/servers/all_codecs/package/package.rel.xml
	@$(MSG) making release file $@ "(because of $(firstword $?))" ...
	-$(RM) $@
	$(call MKRELTAR,package/rel/ti_sdo_ce_examples_servers_all_codecs.xdc.inc,package/rel/ti_sdo_ce_examples_servers_all_codecs.tar.dep)


release release,ti_sdo_ce_examples_servers_all_codecs: all ti_sdo_ce_examples_servers_all_codecs.tar
clean:: .clean
	-$(RM) ti_sdo_ce_examples_servers_all_codecs.tar
	-$(RM) package/rel/ti_sdo_ce_examples_servers_all_codecs.xdc.inc
	-$(RM) package/rel/ti_sdo_ce_examples_servers_all_codecs.tar.dep

clean:: .clean
	-$(RM) .libraries .libraries,*
clean:: 
	-$(RM) .dlls .dlls,*
#
# The following clean rule removes user specified
# generated files or directories.
#
	-$(RMDIR) bin/

ifneq (clean,$(MAKECMDGOALS))
ifeq (,$(wildcard bin))
    $(shell $(MKDIR) bin)
endif
ifeq (,$(wildcard bin/ti_platforms_evmTI816X))
    $(shell $(MKDIR) bin/ti_platforms_evmTI816X)
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
ifeq (,$(wildcard package/cfg/bin))
    $(shell $(MKDIR) package/cfg/bin)
endif
ifeq (,$(wildcard package/cfg/bin/ti_platforms_evmTI816X))
    $(shell $(MKDIR) package/cfg/bin/ti_platforms_evmTI816X)
endif
ifeq (,$(wildcard package/cfg/bin/ti_platforms_evmTI816X/all_DSP))
    $(shell $(MKDIR) package/cfg/bin/ti_platforms_evmTI816X/all_DSP)
endif
endif
clean::
	-$(RMDIR) package


clean:: 
	-$(RM) package/ti.sdo.ce.examples.servers.all_codecs.pjt