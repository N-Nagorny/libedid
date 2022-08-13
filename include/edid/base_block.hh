#pragma once

#include <array>
#include <optional>
#include <string>

#include "common.hh"

#define BASE_START_MANUFACTURE_YEAR 1990

#define BASE_18_BYTE_DESCRIPTORS 4
#define BASE_18_BYTE_DESCRIPTORS_OFFSET 54

#define BASE_DISPLAY_DESCRIPTOR_HEADER_SIZE 5
#define MAX_DISPLAY_NAME_CHARS 12

#define BASE_DISPLAY_DESCRIPTOR_DUMMY_TYPE 0x10
#define BASE_DISPLAY_DESCRIPTOR_RANGE_LIMITS_TYPE 0xFD
#define BASE_DISPLAY_DESCRIPTOR_NAME_TYPE 0xFC
#define BASE_DISPLAY_DESCRIPTOR_SERIAL_NUMBER_TYPE 0xFF

namespace Edid {
  static const std::array<uint8_t, 8> base_block_header = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

  enum BitDepth {
    BD_UNDEFINED = 0b000,
    BD_6 = 0b001,
    BD_8 = 0b010,
    BD_10 = 0b011,
    BD_12 = 0b100,
    BD_14 = 0b101,
    BD_16 = 0b110,
    BD_RESERVED = 0b111
  };

  STRINGIFY_ENUM(BitDepth, {
    {BD_UNDEFINED, "Undefined"},
    {BD_6, "6"},
    {BD_8, "8"},
    {BD_10, "10"},
    {BD_12, "12"},
    {BD_14, "14"},
    {BD_16, "16"},
    {BD_RESERVED, "Reserved"},
  })

  enum VideoInterface {
    UNDEFINED = 0b0000,
    HDMIa = 0b0010,
    HDMIb = 0b0011,
    MDDI = 0b0100,
    DisplayPort = 0b101
  };

  STRINGIFY_ENUM(VideoInterface, {
    {UNDEFINED, "Undefined"},
    {HDMIa, "HDMIa"},
    {HDMIb, "HDMIb"},
    {MDDI, "MDDI"},
    {DisplayPort, "DisplayPort"},
  })

  enum DigitalDisplayType {
    RGB444 = 0b00,
    RGB444_YCrCb444 = 0b01,
    RGB444_YCrCb422 = 0b10,
    RGB444_YCrCb444_YCrCb422 = 0b11
  };

  STRINGIFY_ENUM(DigitalDisplayType, {
    {RGB444, "RGB 4:4:4"},
    {RGB444_YCrCb444, "RGB 4:4:4, YCrCb 4:4:4"},
    {RGB444_YCrCb422, "RGB 4:4:4, YCrCb 4:2:2"},
    {RGB444_YCrCb444_YCrCb422, "RGB 4:4:4, YCrCb 4:4:4, YCrCb 4:2:2"},
  })

  enum EstablishedTiming1 {
    ET_720x400_70 = 1 << 7,
    ET_720x400_88 = 1 << 6,
    ET_640x480_60 = 1 << 5,
    ET_640x480_67 = 1 << 4,
    ET_640x480_72 = 1 << 3,
    ET_640x480_75 = 1 << 2,
    ET_800x600_56 = 1 << 1,
    ET_800x600_60 = 1 << 0
  };

  STRINGIFY_ENUM(EstablishedTiming1, {
    {ET_720x400_70, "720x400@70Hz"},
    {ET_720x400_88, "720x400@88Hz"},
    {ET_640x480_60, "640x480@60Hz"},
    {ET_640x480_67, "640x480@67Hz"},
    {ET_640x480_72, "640x480@72Hz"},
    {ET_640x480_75, "640x480@75Hz"},
    {ET_800x600_56, "800x600@56Hz"},
    {ET_800x600_60, "800x600@60Hz"},
  })

  enum EstablishedTiming2 {
    ET_800x600_72 = 1 << 7,
    ET_800x600_75 = 1 << 6,
    ET_832x624_75 = 1 << 5,
    ET_1024x768_87i = 1 << 4,
    ET_1024x768_60 = 1 << 3,
    ET_1024x768_70 = 1 << 2,
    ET_1024x768_75 = 1 << 1,
    ET_1280x1024_75 = 1 << 0
  };

  STRINGIFY_ENUM(EstablishedTiming2, {
    {ET_800x600_72, "800x600@72Hz"},
    {ET_800x600_75, "800x600@75Hz"},
    {ET_832x624_75, "832x624@75Hz"},
    {ET_1024x768_87i, "1024x768@87Hz interlaced"},
    {ET_1024x768_60, "1024x768@60Hz"},
    {ET_1024x768_70, "1024x768@70Hz"},
    {ET_1024x768_75, "1024x768@75Hz"},
    {ET_1280x1024_75, "1280x1024@75Hz"},
  })

  enum EstablishedTiming3 {
    ET_1152x870_75 = 1 << 7
  };

  STRINGIFY_ENUM(EstablishedTiming3, {
    {ET_1152x870_75, "1152x870@75Hz"},
  })

  using EstablishedTimings = uint8_t;

  enum AspectRatio {
    AR_16_10 = 0b00,
    AR_4_3 = 0b01,
    AR_5_4 = 0b10,
    AR_16_9 = 0b11
  };

  STRINGIFY_ENUM(AspectRatio, {
    {AR_16_10, "16:10"},
    {AR_4_3, "4:3"},
    {AR_5_4, "5:4"},
    {AR_16_9, "16:9"},
  })

  struct StandardTiming {
    uint16_t x_resolution; // This value becomes uint8_t in EDID by dividing by 8 and subtracting 31 from it
    AspectRatio aspect_ratio;
    uint8_t v_frequency;
  };

  bool operator==(const StandardTiming& lhs, const StandardTiming& rhs);

  enum VideoTimingSupport {
    VTS_DEFAULT_GTF = 0x00,
    VTS_BARE_LIMITS = 0x01,
    VTS_SECONDARY_GTF = 0x02,
    VTS_CVT = 0x03
  };

  STRINGIFY_ENUM(VideoTimingSupport, {
    {VTS_DEFAULT_GTF, "GTF"},
    {VTS_BARE_LIMITS, "Bare Limits"},
    {VTS_SECONDARY_GTF, "Secondary GTF"},
    {VTS_CVT, "CVT"},
  })

  struct DisplayRangeLimits {
    uint16_t min_v_rate_hz = 1; // Range is 1..510 Hz with step of 1 Hz
    uint16_t max_v_rate_hz = 1; // Range is 1..510 Hz with step of 1 Hz
    uint16_t min_h_rate_khz = 1; // Range is 1..510 kHz with step of 1 kHz
    uint16_t max_h_rate_khz = 1; // Range is 1..510 kHz with step of 1 kHz
    uint16_t max_pixel_clock_rate_mhz = 10; // Range is 10..2550 MHz with step of 10 MHz
    VideoTimingSupport vts;
  };

  bool operator==(const DisplayRangeLimits& lhs, const DisplayRangeLimits& rhs);

  // See https://en.wikipedia.org/wiki/Extended_Display_Identification_Data
  struct BaseBlock {
    // Header
    std::array<char, 3> manufacturer_id;
    uint16_t manufacturer_product_code = 0;
    uint32_t serial_number = 0;
    uint8_t manufacture_week = 0;
    uint16_t manufacture_year = 2000;  // This value becomes uint8_t in EDID by subtracting 1990 from it
    uint8_t edid_major_version = 1;
    uint8_t edid_minor_version = 4;
    // Basic Display Parameters
    // NOTE: Only Digital Input supported
    BitDepth bits_per_color = BitDepth::BD_UNDEFINED;
    VideoInterface video_interface = VideoInterface::UNDEFINED;
    uint8_t h_screen_size = 0;        // Horizontal screen size in cm - usually just choose values to set aspect ratio
    uint8_t v_screen_size = 0;
    uint8_t gamma = 0;
    bool dpms_standby = false;
    bool dpms_suspend = false;
    bool dpms_active_off = false;
    DigitalDisplayType display_type = DigitalDisplayType::RGB444;
    bool standard_srgb = false;
    bool preferred_timing_mode = true;
    bool continuous_timings = false;

    std::array<uint8_t, 10> chromaticity;

    // Established Timings
    EstablishedTimings established_timings_1 = 0x0;
    EstablishedTimings established_timings_2 = 0x0;
    EstablishedTimings established_timings_3 = 0x0;

    std::array<std::optional<StandardTiming>, 8> standard_timings;
    std::array<std::optional<DetailedTimingDescriptor>, 2> detailed_timing_descriptors;
    std::optional<DisplayRangeLimits> display_range_limits;
    std::optional<std::string> display_name; // Maximum is 12 chars
    std::optional<std::string> display_serial_number; // Maximum is 12 chars

    BaseBlock() {
      manufacturer_id.fill('A');
      chromaticity.fill(0);
    }
  };

  bool operator==(const BaseBlock& lhs, const BaseBlock& rhs);

  /** Generates EDID Base Block binary */
  std::array<uint8_t, EDID_BLOCK_SIZE> generate_base_block(
    const BaseBlock& base_block, /**< Base Block structure */
    uint8_t ext_blocks /**< Number of extension blocks following the Base Block in effective EDID binary */
  );

  std::optional<StandardTiming> parse_standard_timing(uint8_t byte_1, uint8_t byte_2);

  /** Parses EDID Base Block binary into Base Block structure and number of extension blocks in EDID binary */
  std::pair<BaseBlock, uint8_t> parse_base_block(
    const std::array<uint8_t, EDID_BLOCK_SIZE>& base_block /**< EDID Base Block binary */
  );

  void print_standard_timing(
    std::ostream& os, /**< Output stream */
    const StandardTiming& std_timing /**< Standard Timing structure */
  );

  /** Prints Base Block structure into stream */
  void print_base_block(
    std::ostream& os, /**< Output stream */
    const BaseBlock& base_block /**< Base Block structure */
  );
}
