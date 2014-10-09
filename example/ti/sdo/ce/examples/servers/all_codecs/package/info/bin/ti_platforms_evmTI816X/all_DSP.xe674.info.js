{
    "server algorithms": {
        "programName": "bin/ti_platforms_evmTI816X/all_DSP.xe674",
        "algs": [
            {
                "name": "viddec_copy",
                "pkg": "ti.sdo.ce.examples.codecs.viddec_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.viddec_copy.VIDDEC_COPY",
                "threadAttrs": {
                    "priority": 3,
                },
                "groupId": 0,
                "ialgFxns": "VIDDECCOPY_TI_VIDDECCOPY",
                "serverFxns": "VIDDEC_SKEL",
                "stubFxns": "VIDDEC_STUBS",
                "rpcProtocolVersion": 3,
            },
            {
                "name": "videnc_copy",
                "pkg": "ti.sdo.ce.examples.codecs.videnc_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.videnc_copy.VIDENC_COPY",
                "threadAttrs": {
                    "priority": 3,
                },
                "groupId": 0,
                "ialgFxns": "VIDENCCOPY_TI_VIDENCCOPY",
                "idma3Fxns": "VIDENCCOPY_TI_IDMA3",
                "serverFxns": "VIDENC_SKEL",
                "stubFxns": "VIDENC_STUBS",
                "rpcProtocolVersion": 3,
            },
            {
                "name": "imgdec_copy",
                "pkg": "ti.sdo.ce.examples.codecs.imgdec_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.imgdec_copy.IMGDEC_COPY",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "IMGDECCOPY_TI_IMGDECCOPY",
                "serverFxns": "IMGDEC_SKEL",
                "stubFxns": "IMGDEC_STUBS",
                "rpcProtocolVersion": 3,
            },
            {
                "name": "imgenc_copy",
                "pkg": "ti.sdo.ce.examples.codecs.imgenc_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.imgenc_copy.IMGENC_COPY",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "IMGENCCOPY_TI_IMGENCCOPY",
                "serverFxns": "IMGENC_SKEL",
                "stubFxns": "IMGENC_STUBS",
                "rpcProtocolVersion": 3,
            },
            {
                "name": "auddec_copy",
                "pkg": "ti.sdo.ce.examples.codecs.auddec_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.auddec_copy.AUDDEC_COPY",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "AUDDECCOPY_TI_IAUDDECCOPY",
                "serverFxns": "AUDDEC_SKEL",
                "stubFxns": "AUDDEC_STUBS",
                "rpcProtocolVersion": 3,
            },
            {
                "name": "speexenc",
                "pkg": "ti.sdo.ce.examples.codecs.speexenc",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.speexenc.speexenc",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "speexenc_TI_IALG",
                "serverFxns": "AUDENC_SKEL",
                "stubFxns": "AUDENC_STUBS",
                "rpcProtocolVersion": 3,
            },
            {
                "name": "audenc_copy",
                "pkg": "ti.sdo.ce.examples.codecs.audenc_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.audenc_copy.AUDENC_COPY",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "AUDENCCOPY_TI_AUDENCCOPY",
                "serverFxns": "AUDENC_SKEL",
                "stubFxns": "AUDENC_STUBS",
                "rpcProtocolVersion": 3,
            },
            {
                "name": "sphenc_copy",
                "pkg": "ti.sdo.ce.examples.codecs.sphenc_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.sphenc_copy.SPHENC_COPY",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "SPHENCCOPY_TI_SPHENCCOPY",
                "serverFxns": "SPHENC_SKEL",
                "stubFxns": "SPHENC_STUBS",
                "rpcProtocolVersion": 3,
            },
            {
                "name": "sphdec_copy",
                "pkg": "ti.sdo.ce.examples.codecs.sphdec_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.sphdec_copy.SPHDEC_COPY",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "SPHDECCOPY_TI_SPHDECCOPY",
                "serverFxns": "SPHDEC_SKEL",
                "stubFxns": "SPHDEC_STUBS",
                "rpcProtocolVersion": 3,
            },
            {
                "name": "scale",
                "pkg": "ti.sdo.ce.examples.codecs.scale",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.scale.SCALE_TI",
                "threadAttrs": {
                    "priority": 2,
                },
                "ialgFxns": "SCALE_TI_SCALE",
                "serverFxns": "SCALE_SKEL",
                "stubFxns": "SCALE_STUBS",
                "rpcProtocolVersion": 1,
            },
            {
                "name": "viddec2_copy",
                "pkg": "ti.sdo.ce.examples.codecs.viddec2_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.viddec2_copy.VIDDEC2_COPY",
                "threadAttrs": {
                    "priority": 3,
                },
                "groupId": 0,
                "ialgFxns": "VIDDEC2COPY_TI_IVIDDEC2COPY",
                "serverFxns": "VIDDEC2_SKEL",
                "stubFxns": "VIDDEC2_STUBS",
                "rpcProtocolVersion": 0,
            },
            {
                "name": "videnc1_copy",
                "pkg": "ti.sdo.ce.examples.codecs.videnc1_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.videnc1_copy.VIDENC1_COPY",
                "threadAttrs": {
                    "priority": 3,
                },
                "groupId": 0,
                "ialgFxns": "VIDENC1COPY_TI_IVIDENC1COPY",
                "idma3Fxns": null,
                "serverFxns": "VIDENC1_SKEL",
                "stubFxns": "VIDENC1_STUBS",
                "rpcProtocolVersion": 0,
            },
            {
                "name": "imgdec1_copy",
                "pkg": "ti.sdo.ce.examples.codecs.imgdec1_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.imgdec1_copy.IMGDEC1_COPY",
                "threadAttrs": {
                    "priority": 2,
                },
                "ialgFxns": "IMGDEC1COPY_TI_IIMGDEC1COPY",
                "serverFxns": "IMGDEC1_SKEL",
                "stubFxns": "IMGDEC1_STUBS",
                "rpcProtocolVersion": 1,
            },
            {
                "name": "imgenc1_copy",
                "pkg": "ti.sdo.ce.examples.codecs.imgenc1_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.imgenc1_copy.IMGENC1_COPY",
                "threadAttrs": {
                    "priority": 2,
                },
                "ialgFxns": "IMGENC1COPY_TI_IIMGENC1COPY",
                "serverFxns": "IMGENC1_SKEL",
                "stubFxns": "IMGENC1_STUBS",
                "rpcProtocolVersion": 1,
            },
            {
                "name": "sphdec1_copy",
                "pkg": "ti.sdo.ce.examples.codecs.sphdec1_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.sphdec1_copy.SPHDEC1_COPY",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "SPHDEC1COPY_TI_ISPHDEC1COPY",
                "serverFxns": "SPHDEC1_SKEL",
                "stubFxns": "SPHDEC1_STUBS",
                "rpcProtocolVersion": 0,
            },
            {
                "name": "sphenc1_copy",
                "pkg": "ti.sdo.ce.examples.codecs.sphenc1_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.sphenc1_copy.SPHENC1_COPY",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "SPHENC1COPY_TI_ISPHENC1COPY",
                "serverFxns": "SPHENC1_SKEL",
                "stubFxns": "SPHENC1_STUBS",
                "rpcProtocolVersion": 0,
            },
            {
                "name": "auddec1_copy",
                "pkg": "ti.sdo.ce.examples.codecs.auddec1_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.auddec1_copy.AUDDEC1_COPY",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "AUDDEC1COPY_TI_IAUDDEC1COPY",
                "serverFxns": "AUDDEC1_SKEL",
                "stubFxns": "AUDDEC1_STUBS",
                "rpcProtocolVersion": 1,
            },
            {
                "name": "audenc1_copy",
                "pkg": "ti.sdo.ce.examples.codecs.audenc1_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.audenc1_copy.AUDENC1_COPY",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "AUDENC1COPY_TI_IAUDENC1COPY",
                "serverFxns": "AUDENC1_SKEL",
                "stubFxns": "AUDENC1_STUBS",
                "rpcProtocolVersion": 1,
            },
            {
                "name": "g711enc",
                "pkg": "ti.sdo.ce.examples.codecs.g711",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.g711.G711ENC",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "G711ENC_SUN_IG711ENC",
                "serverFxns": "SPHENC1_SKEL",
                "stubFxns": "SPHENC1_STUBS",
                "rpcProtocolVersion": 0,
            },
            {
                "name": "g711dec",
                "pkg": "ti.sdo.ce.examples.codecs.g711",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.g711.G711DEC",
                "threadAttrs": {
                    "priority": 4,
                },
                "ialgFxns": "G711DEC_SUN_IG711DEC",
                "serverFxns": "SPHDEC1_SKEL",
                "stubFxns": "SPHDEC1_STUBS",
                "rpcProtocolVersion": 0,
            },
            {
                "name": "vidtranscode_copy",
                "pkg": "ti.sdo.ce.examples.codecs.vidtranscode_copy",
                "pkgVersion": [
                ],
                "mod": "ti.sdo.ce.examples.codecs.vidtranscode_copy.VIDTRANSCODE_COPY",
                "threadAttrs": {
                    "priority": 2,
                },
                "ialgFxns": "VIDTRANSCODECOPY_TI_IVIDTRANSCODECOPY",
                "serverFxns": "VIDTRANSCODE_SKEL",
                "stubFxns": "VIDTRANSCODE_STUBS",
                "rpcProtocolVersion": 1,
            },
            {
                "name": "vidanalytics_copy",
                "pkg": "ti.sdo.ce.examples.codecs.vidanalytics_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.vidanalytics_copy.VIDANALYTICS_COPY",
                "threadAttrs": {
                    "priority": 2,
                },
                "ialgFxns": "VIDANALYTICSCOPY_TI_IVIDANALYTICSCOPY",
                "serverFxns": "VIDANALYTICS_SKEL",
                "stubFxns": "VIDANALYTICS_STUBS",
                "rpcProtocolVersion": 1,
            },
            {
                "name": "viddec1_copy",
                "pkg": "ti.sdo.ce.examples.codecs.viddec1_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.viddec1_copy.VIDDEC1_COPY",
                "threadAttrs": {
                    "priority": 3,
                },
                "groupId": 0,
                "ialgFxns": "VIDDEC1COPY_TI_IVIDDEC1COPY",
                "serverFxns": "VIDDEC1_SKEL",
                "stubFxns": "VIDDEC1_STUBS",
                "rpcProtocolVersion": 0,
            },
            {
                "name": "viddec2back_copy",
                "pkg": "ti.sdo.ce.examples.codecs.viddec2split_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.viddec2split_copy.VIDDEC2BACK_COPY",
                "threadAttrs": {
                    "priority": 3,
                },
                "groupId": 0,
                "ialgFxns": "VIDDEC2BACKCOPY_TI_IVIDDEC2BACKCOPY",
                "serverFxns": "VIDDEC2BACK_SKEL",
                "stubFxns": "VIDDEC2BACK_STUBS",
                "rpcProtocolVersion": 0,
            },
            {
                "name": "universal_copy",
                "pkg": "ti.sdo.ce.examples.codecs.universal_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.universal_copy.UNIVERSAL_COPY",
                "threadAttrs": {
                    "priority": 2,
                },
                "ialgFxns": "UNIVERSALCOPY_TI_IUNIVERSALCOPY",
                "serverFxns": "UNIVERSAL_SKEL",
                "stubFxns": "UNIVERSAL_STUBS",
                "rpcProtocolVersion": 1,
            },
            {
                "name": "videnc2_copy",
                "pkg": "ti.sdo.ce.examples.codecs.videnc2_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.videnc2_copy.VIDENC2_COPY",
                "threadAttrs": {
                    "priority": 2,
                },
                "groupId": 0,
                "ialgFxns": "VIDENC2COPY_TI_IVIDENC2COPY",
                "serverFxns": "VIDENC2_SKEL",
                "stubFxns": "VIDENC2_STUBS",
                "rpcProtocolVersion": 0,
            },
            {
                "name": "viddec3_copy",
                "pkg": "ti.sdo.ce.examples.codecs.viddec3_copy",
                "pkgVersion": [
                    "1, 0, 0",
                ],
                "mod": "ti.sdo.ce.examples.codecs.viddec3_copy.VIDDEC3_COPY",
                "threadAttrs": {
                    "priority": 2,
                },
                "groupId": 0,
                "ialgFxns": "VIDDEC3COPY_TI_IVIDDEC3COPY",
                "serverFxns": "VIDDEC3_SKEL",
                "stubFxns": "VIDDEC3_STUBS",
                "rpcProtocolVersion": 0,
            },
        ],
    },
}
