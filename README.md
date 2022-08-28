# C++ EDID library

This is a C++ library for parsing and generating EDID binaries. It's targeted against E-EDID Release A, Revision 2 with EDID structure 1.4 and CTA Extension Version 3 though it doesn't guarantee full support of them.

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

- Model Year is not supported.
- Setting Aspect Ratio instead of Screen Size is not supported.
- Incorrect Video input parameters bitmap and Display type for analog input type. (TODO: std::variant to distinct)
- Display Range Limits Display Descriptor doesn't describe Secondary GTF curve and CVT data.
- Chromaticity is not deserialized.
- Gamma is printed in the floating-point format though BaseBlock stores it in the integer format.

### CTA Extension

- Indicating DTDs and VICs as native is not supported.
- Data Block Collection supports only Video Data Block, Audio Data Block and Speaker Allocation Data Block.
- Format-specific parameters of Short Audio Descriptors are incorrect for all the audio formats except of Linear PCM.
