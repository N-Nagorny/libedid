# C++ EDID library

This is a C++ library for parsing and generating EDID binaries. It's targeted against [E-EDID Release A, Revision 2][E-EDID] with EDID structure 1.4 and CTA Extension Version 3 (as described in [CTA-861-I][CTA-861-I]) however the full support of them is not guaranteed.

Besides providing interfaces for working with EDID binaries in C++ applications, `libedid` provides bidirectional JSON adapters in order to achieve a language-independent interface.

This code is written for educational purposes. Use it on your risk.

## Build procedure

The project uses [CMake](https://cmake.org/) as a build tool and [nix](https://nixos.org/) as a package manager.

Here is an example of build procedure for Linux:
```sh
nix-shell
cmake -B build/ .
cmake --build build/
```

Or just invoke
```
nix-build
```
and build output will appear in `result/`.

## Supported blocks

The list of supported blocks of Base EDID is [here](include/edid/eighteen_byte_descriptor.hh) in `EighteenByteDescriptor`.

The list of supported blocks of CTA Extension is [here](include/edid/cta861_block.hh) in `CtaDataBlock`.

## Known issues

### Development-related (TODO)

- GitHub Actions workflows could be cleaned up.
- Some cpplint checks are skipped (see TODOs in `scripts/static_analysis.sh`).
- `src/json.cc` has too much boilerplate code.
- `linuxhw/EDID` coverage could be indicated via GitHub Badges.
- Rework Established Timings III multi-byte enums in the Colorimetry Data Block style.
- Unify using of optional values in JSON Schemas.

### Base EDID

- Setting Aspect Ratio instead of Screen Size is not supported.
- Incorrect Video input parameters bitmap and Display type for analog input type. (TODO: std::variant to distinct)
- Display Range Limits Display Descriptor doesn't describe Secondary GTF curve and CVT data.
- Chromaticity is not deserialized.
- Gamma is printed in the floating-point format though BaseBlock stores it in the integer format.
- YCbCr color encodings are named as YCrCb (in [the spec also][E-EDID], maybe due to the order mentioned in [BT.601][BT.601]).
- ID Serial Number always exists even if it's all zero.
- No `UnknownDataBlock` for Base EDID.

### CTA Extension

- Format-specific parameters of Short Audio Descriptors are incorrect for any audio formats except of Linear PCM.

[E-EDID]: https://vesa.org/vesa-standards/
[CTA-861-I]: https://shop.cta.tech/collections/standards/products/a-dtv-profile-for-uncompressed-high-speed-digital-interfaces-cta-861-i-errata
[BT.601]: https://www.itu.int/rec/R-REC-BT.601/
