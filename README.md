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

Or just invoke
```
nix-build
```
and build output will appear in `result/`.

## Known issues

### Development-related (TODO)

- CMake doesn't regenerate C++ with JSON Schemas when they're changed.
- `std::variant` is used for writing generalized functions working with any CTA data blocks, mostly because of `parse_byte_block` template functions. Need to consider using inheritance instead.
- Comparison operator overloadings could be replaced with a macro.
- No pre-commit hooks and/or CI procedures that run `cpplint`, `cppcheck`, `test-runner` and `edid-workshop`.
- No `.editorconfig`.
- `src/json.cc` has too much boilerplate code.

### Base EDID

- Setting Aspect Ratio instead of Screen Size is not supported.
- Incorrect Video input parameters bitmap and Display type for analog input type. (TODO: std::variant to distinct)
- Display Range Limits Display Descriptor doesn't describe Secondary GTF curve and CVT data.
- Chromaticity is not deserialized.
- Gamma is printed in the floating-point format though BaseBlock stores it in the integer format.
- YCbCr color encodings are named as YCrCb (in [the spec also][E-EDID], maybe due to the order mentioned in [BT.601][BT.601]).
- ID Serial Number always exists even if it's all zero.

### CTA Extension

- Format-specific parameters of Short Audio Descriptors are incorrect for any audio formats except of Linear PCM.

## Code quality
```
cpplint \
  --root=$(pwd) \
  --exclude=build/* \
  --exclude=result/* \
  --filter=-whitespace/line_length,-build/namespaces,-build/include_order,-whitespace/parens,-whitespace/newline,-readability/braces,-build/explicit_make_pair,-runtime/references,-readability/casting,-runtime/indentation_namespace \
  $(find . -name \*.hh -o -name \*.cc | xargs echo)
```

[E-EDID]: https://vesa.org/vesa-standards/
[CTA-861-G]: https://shop.cta.tech/products/a-dtv-profile-for-uncompressed-high-speed-digital-interfaces-cta-861-g
[BT.601]: https://www.itu.int/rec/R-REC-BT.601/
