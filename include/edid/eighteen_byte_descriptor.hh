#pragma once

#define EIGHTEEN_BYTES 18

#define BASE_FAKE_DTD_TYPE 0x00 // may intersect with manufacturer reserved display descriptor
#define BASE_DISPLAY_DESCRIPTOR_DUMMY_TYPE 0x10
#define BASE_DISPLAY_DESCRIPTOR_ESTABLISHED_TIMINGS_III_TYPE 0xF7
#define BASE_DISPLAY_DESCRIPTOR_RANGE_LIMITS_TYPE 0xFD

namespace Edid {
  struct DetailedTimingDescriptor;
  struct DisplayRangeLimits;
  struct AsciiString;
  struct DummyDescriptor;
  struct EstablishedTimings3;

  using EighteenByteDescriptor = std::variant<
    DetailedTimingDescriptor, DisplayRangeLimits, AsciiString,
    DummyDescriptor, EstablishedTimings3
  >;

  static auto is_dtd_visitor = [](const auto& descriptor) -> bool {
    return descriptor.type() == BASE_FAKE_DTD_TYPE;
  };

  static auto is_et3_visitor = [](const auto& descriptor) -> bool {
    return descriptor.type() == BASE_DISPLAY_DESCRIPTOR_ESTABLISHED_TIMINGS_III_TYPE;
  };
}
