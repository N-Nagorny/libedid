#include "edid/eighteen_byte_descriptor.hh"

#include "edid/base_block.hh"
#include "edid/common.hh"
#include "edid/exceptions.hh"

#include <string>

namespace Edid {
  std::shared_ptr<IEighteenByteDescriptor> IEighteenByteDescriptor::parse_byte_block(const uint8_t* start) {
    if (*start == 0x0 && *(start + 1) == 0x0 && *(start + 2) == 0x0) {
      const uint8_t display_descriptor_type = *(start + 3);
      switch(display_descriptor_type) {
        case BASE_DISPLAY_DESCRIPTOR_RANGE_LIMITS_TYPE:
          return DisplayRangeLimits::parse_byte_block(start);
        case ASCII_DISPLAY_NAME:
        case ASCII_UNSPECIFIED_TEXT:
        case ASCII_SERIAL_NUMBER:
          return AsciiString::parse_byte_block(start);
        case BASE_DISPLAY_DESCRIPTOR_ESTABLISHED_TIMINGS_III_TYPE:
          return EstablishedTimings3::parse_byte_block(start);
        case BASE_DISPLAY_DESCRIPTOR_DUMMY_TYPE:
          return std::make_unique<DummyDescriptor>();
        default:
          throw EdidException(__FUNCTION__,
            "Display Descriptor has unknown Display Descriptor Type " +
            std::to_string(display_descriptor_type)
          );
      }
    }
    return DetailedTimingDescriptor::parse_byte_block(start);
  }
}
