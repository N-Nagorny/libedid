# C++ EDID library

This is a C++ library for parsing and generating EDID binaries. It's targeted against [E-EDID Release A, Revision 2][E-EDID] with EDID structure 1.4 and CTA Extension Version 3 (as described in [CTA-861-G][CTA-861-G]) though it doesn't guarantee full support of them.

This code is written for educational purposes. Use it on your risk.

## Build procedure

The project uses [CMake](https://cmake.org/) as a build tool and [nix](https://nixos.org/) as a package manager.

Here is an example of build procedure for Linux:
```sh
nix-shell
cmake -B build/ .
cmake --build build/
```

## Known issues

### Base EDID

- Setting Aspect Ratio instead of Screen Size is not supported.
- Incorrect Video input parameters bitmap and Display type for analog input type. (TODO: std::variant to distinct)
- Display Range Limits Display Descriptor doesn't describe Secondary GTF curve and CVT data.
- Chromaticity is not deserialized.
- Gamma is printed in the floating-point format though BaseBlock stores it in the integer format.
- YCbCr color encodings are named as YCrCb (in [the spec also][E-EDID], maybe due to the order mentioned in [BT.601][BT.601]).
- ID Serial Number always exists even if it's all zero.

### CTA Extension

- Indicating DTDs and VICs as native is not supported.
- Format-specific parameters of Short Audio Descriptors are incorrect for any audio formats except of Linear PCM.

[E-EDID]: https://vesa.org/vesa-standards/
[CTA-861-G]: https://shop.cta.tech/products/a-dtv-profile-for-uncompressed-high-speed-digital-interfaces-cta-861-g
[BT.601]: https://www.itu.int/rec/R-REC-BT.601/
