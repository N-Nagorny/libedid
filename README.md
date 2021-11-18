# C++ EDID library

This is a C++ library for parsing and generating EDID binaries. It's targeted against E-EDID Release A, Revision 2 with EDID structure 1.4 and CTA Extension Version 3 though it doesn't guarantee full support of them.

This code is written for educational purposes. Use it on your risk.

## Known issues

### Base EDID

- Incorrect Video input parameters bitmap and Display type for analog input type.
- Display Range Limits Display Descriptor always has no timing information.
- Chromaticity is not deserialized.
- Features bitmap in DTD is not deserialized.
- Number of extensions is not parsed from Base EDID.
