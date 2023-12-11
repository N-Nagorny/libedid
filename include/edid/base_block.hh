// Copyright 2023 N-Nagorny
#pragma once

#include <array>
#include <optional>
#include <string>
#include <utility>

#ifdef ENABLE_JSON
#include <nlohmann/json.hpp>
#endif

#include "common.hh"

#define BASE_START_MANUFACTURE_YEAR 1990

#define BASE_18_BYTE_DESCRIPTORS 4

#define BASE_DISPLAY_DESCRIPTOR_HEADER_SIZE 5
#define MAX_ASCII_STRING_LENGTH 13

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
    VI_UNDEFINED = 0b0000,
    VI_DVI = 0b0001,
    VI_HDMIa = 0b0010,
    VI_HDMIb = 0b0011,
    VI_MDDI = 0b0100,
    VI_DisplayPort = 0b101
  };

  STRINGIFY_ENUM(VideoInterface, {
    {VI_UNDEFINED, "Undefined"},
    {VI_DVI, "DVI"},
    {VI_HDMIa, "HDMIa"},
    {VI_HDMIb, "HDMIb"},
    {VI_MDDI, "MDDI"},
    {VI_DisplayPort, "DisplayPort"},
  })

  enum DigitalDisplayType {
    DDT_RGB444 = 0b00,
    DDT_RGB444_YCrCb444 = 0b01,
    DDT_RGB444_YCrCb422 = 0b10,
    DDT_RGB444_YCrCb444_YCrCb422 = 0b11
  };

  STRINGIFY_ENUM(DigitalDisplayType, {
    {DDT_RGB444, "RGB 4:4:4"},
    {DDT_RGB444_YCrCb444, "RGB 4:4:4, YCrCb 4:4:4"},
    {DDT_RGB444_YCrCb422, "RGB 4:4:4, YCrCb 4:2:2"},
    {DDT_RGB444_YCrCb444_YCrCb422, "RGB 4:4:4, YCrCb 4:4:4, YCrCb 4:2:2"},
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
    {ET_800x600_60, "800x600@60Hz"}
  })

  enum EstablishedTiming2 {
    ET_800x600_72   = 1 << 7,
    ET_800x600_75   = 1 << 6,
    ET_832x624_75   = 1 << 5,
    ET_1024x768i_87 = 1 << 4,
    ET_1024x768_60  = 1 << 3,
    ET_1024x768_70  = 1 << 2,
    ET_1024x768_75  = 1 << 1,
    ET_1280x1024_75 = 1 << 0
  };

  STRINGIFY_ENUM(EstablishedTiming2, {
    {ET_800x600_72, "800x600@72Hz"},
    {ET_800x600_75, "800x600@75Hz"},
    {ET_832x624_75, "832x624@75Hz"},
    {ET_1024x768i_87, "1024x768i@87Hz"},
    {ET_1024x768_60, "1024x768@60Hz"},
    {ET_1024x768_70, "1024x768@70Hz"},
    {ET_1024x768_75, "1024x768@75Hz"},
    {ET_1280x1024_75, "1280x1024@75Hz"}
  })

  enum ManufacturersTiming {
    ET_1152x870_75 = 1 << 7
  };

  STRINGIFY_ENUM(ManufacturersTiming, {
    {ET_1152x870_75, "1152x870@75Hz"}
  })

  enum EstablishedTiming3Byte6 {
    ET_3_6_NOT_FOUND = ENUM_NULL,
    ET_640x350_85    = 1 << 7,
    ET_640x400_85    = 1 << 6,
    ET_720x400_85    = 1 << 5,
    ET_640x480_85    = 1 << 4,
    ET_848x480_60    = 1 << 3,
    ET_800x600_85    = 1 << 2,
    ET_1024x768_85   = 1 << 1,
    ET_1152x864_75   = 1 << 0
  };

  STRINGIFY_ENUM(EstablishedTiming3Byte6, {
    {ET_3_6_NOT_FOUND,   "!!! NO VALUE !!!"},
    {ET_640x350_85,  "640x350@85Hz"},
    {ET_640x400_85,  "640x400@85Hz"},
    {ET_720x400_85,  "720x400@85Hz"},
    {ET_640x480_85,  "640x480@85Hz"},
    {ET_848x480_60,  "848x480@60Hz"},
    {ET_800x600_85,  "800x600@85Hz"},
    {ET_1024x768_85, "1024x768@85Hz"},
    {ET_1152x864_75, "1152x864@75Hz"}
  })

  enum EstablishedTiming3Byte7 {
    ET_3_7_NOT_FOUND  = ENUM_NULL,
    ET_1280x768_60_RB = 1 << 7,
    ET_1280x768_60    = 1 << 6,
    ET_1280x768_75    = 1 << 5,
    ET_1280x768_85    = 1 << 4,
    ET_1280x960_60    = 1 << 3,
    ET_1280x960_85    = 1 << 2,
    ET_1280x1024_60   = 1 << 1,
    ET_1280x1024_85   = 1 << 0
  };

  STRINGIFY_ENUM(EstablishedTiming3Byte7, {
    {ET_3_7_NOT_FOUND,   "!!! NO VALUE !!!"},
    {ET_1280x768_60_RB,  "1280x768@60Hz_RB"},
    {ET_1280x768_60,     "1280x768@60Hz"},
    {ET_1280x768_75,     "1280x768@75Hz"},
    {ET_1280x768_85,     "1280x768@85Hz"},
    {ET_1280x960_60,     "1280x960@60Hz"},
    {ET_1280x960_85,     "1280x960@85Hz"},
    {ET_1280x1024_60,    "1280x1024@60Hz"},
    {ET_1280x1024_85,    "1280x1024@85Hz"}
  })

  enum EstablishedTiming3Byte8 {
    ET_3_8_NOT_FOUND = ENUM_NULL,
    ET_1360x768_60 = 1 << 7,
    ET_1440x900_60_RB = 1 << 6,
    ET_1440x900_60 = 1 << 5,
    ET_1440x900_75 = 1 << 4,
    ET_1440x900_85 = 1 << 3,
    ET_1440x1050_60_RB = 1 << 2,
    ET_1440x1050_60 = 1 << 1,
    ET_1440x1050_75 = 1 << 0
  };

  STRINGIFY_ENUM(EstablishedTiming3Byte8, {
    {ET_3_8_NOT_FOUND,   "!!! NO VALUE !!!"},
    {ET_1360x768_60,     "1360x768@60Hz"},
    {ET_1440x900_60_RB,  "1440x900@60Hz_RB"},
    {ET_1440x900_60,     "1440x900@60Hz"},
    {ET_1440x900_75,     "1440x900@75Hz"},
    {ET_1440x900_85,     "1440x900@85Hz"},
    {ET_1440x1050_60_RB, "1440x1050@60Hz_RB"},
    {ET_1440x1050_60,    "1440x1050@60Hz"},
    {ET_1440x1050_75,    "1440x1050@75Hz"}
  })

  enum EstablishedTiming3Byte9 {
    ET_3_9_NOT_FOUND = ENUM_NULL,
    ET_1400x1050_85 = 1 << 7,
    ET_1680x1050_60_RB = 1 << 6,
    ET_1680x1050_60 = 1 << 5,
    ET_1680x1050_75 = 1 << 4,
    ET_1680x1050_85 = 1 << 3,
    ET_1600x1200_60 = 1 << 2,
    ET_1600x1200_65 = 1 << 1,
    ET_1600x1200_70 = 1 << 0
  };

  STRINGIFY_ENUM(EstablishedTiming3Byte9, {
    {ET_3_9_NOT_FOUND,   "!!! NO VALUE !!!"},
    {ET_1400x1050_85,     "1400x1050@85Hz"},
    {ET_1680x1050_60_RB,  "1680x1050@60Hz_RB"},
    {ET_1680x1050_60,     "1680x1050@60Hz"},
    {ET_1680x1050_75,     "1680x1050@75Hz"},
    {ET_1680x1050_85,     "1680x1050@85Hz"},
    {ET_1600x1200_60,     "1600x1200@60Hz"},
    {ET_1600x1200_65,     "1600x1200@65Hz"},
    {ET_1600x1200_70,     "1600x1200@70Hz"}
  })

  enum EstablishedTiming3Byte10 {
    ET_3_10_NOT_FOUND = ENUM_NULL,
    ET_1600x1200_75 = 1 << 7,
    ET_1600x1200_85 = 1 << 6,
    ET_1792x1344_60 = 1 << 5,
    ET_1792x1344_75 = 1 << 4,
    ET_1856x1392_60 = 1 << 3,
    ET_1856x1392_75 = 1 << 2,
    ET_1920x1200_60_RB = 1 << 1,
    ET_1920x1200_60 = 1 << 0
  };

  STRINGIFY_ENUM(EstablishedTiming3Byte10, {
    {ET_3_10_NOT_FOUND,   "!!! NO VALUE !!!"},
    {ET_1600x1200_75,     "1600x1200@75Hz"},
    {ET_1600x1200_85,     "1600x1200@85Hz"},
    {ET_1792x1344_60,     "1792x1344@60Hz"},
    {ET_1792x1344_75,     "1792x1344@75Hz"},
    {ET_1856x1392_60,     "1856x1392@60Hz"},
    {ET_1856x1392_75,     "1856x1392@75Hz"},
    {ET_1920x1200_60_RB,  "1920x1200@60Hz_RB"},
    {ET_1920x1200_60,     "1920x1200@60Hz"}
  })

  enum EstablishedTiming3Byte11 {
    ET_3_11_NOT_FOUND = ENUM_NULL,
    ET_1920x1200_75 = 1 << 7,
    ET_1920x1200_85 = 1 << 6,
    ET_1920x1440_60 = 1 << 5,
    ET_1920x1440_75 = 1 << 4
  };

  STRINGIFY_ENUM(EstablishedTiming3Byte11, {
    {ET_3_11_NOT_FOUND,   "!!! NO VALUE !!!"},
    {ET_1920x1200_75,     "1920x1200@75Hz"},
    {ET_1920x1200_85,     "1920x1200@85Hz"},
    {ET_1920x1440_60,     "1920x1440@60Hz"},
    {ET_1920x1440_75,     "1920x1440@75Hz"}
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
    uint16_t x_resolution;  // This value becomes uint8_t in EDID by dividing by 8 and subtracting 31 from it
    AspectRatio aspect_ratio;
    uint8_t v_frequency;

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(StandardTiming, x_resolution, aspect_ratio, v_frequency)
#endif
  };

#define FIELDS(X) X.x_resolution, X.aspect_ratio, X.v_frequency
  TIED_COMPARISONS(StandardTiming, FIELDS)
#undef FIELDS

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

  struct DisplayRangeLimits : IEighteenByteDescriptor {
    uint16_t min_v_rate_hz = 1;  // Range is 1..510 Hz with step of 1 Hz
    uint16_t max_v_rate_hz = 1;  // Range is 1..510 Hz with step of 1 Hz
    uint16_t min_h_rate_khz = 1;  // Range is 1..510 kHz with step of 1 kHz
    uint16_t max_h_rate_khz = 1;  // Range is 1..510 kHz with step of 1 kHz
    uint16_t max_pixel_clock_rate_mhz = 10;  // Range is 10..2550 MHz with step of 10 MHz
    VideoTimingSupport video_timing_support = VideoTimingSupport::VTS_DEFAULT_GTF;

    DisplayRangeLimits() = default;

    // for brace-enclosed initialization despite
    // inheritance from a base class with virtual funcs
    DisplayRangeLimits(
      uint16_t min_v_rate_hz_,
      uint16_t max_v_rate_hz_,
      uint16_t min_h_rate_khz_,
      uint16_t max_h_rate_khz_,
      uint16_t max_pixel_clock_rate_mhz_,
      VideoTimingSupport video_timing_support_
    )
      : min_v_rate_hz(min_v_rate_hz_)
      , max_v_rate_hz(max_v_rate_hz_)
      , min_h_rate_khz(min_h_rate_khz_)
      , max_h_rate_khz(max_h_rate_khz_)
      , max_pixel_clock_rate_mhz(max_pixel_clock_rate_mhz_)
      , video_timing_support(video_timing_support_)
    {}

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DisplayRangeLimits,
      min_v_rate_hz,
      max_v_rate_hz,
      min_h_rate_khz,
      max_h_rate_khz,
      max_pixel_clock_rate_mhz,
      video_timing_support
    )
#endif

    uint8_t type() const override {
      return BASE_DISPLAY_DESCRIPTOR_RANGE_LIMITS_TYPE;
    }

    std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    static std::shared_ptr<DisplayRangeLimits> parse_byte_block(const uint8_t* start) {
      DisplayRangeLimits result;
      int pos = 0;

      bool add_255_to_h_max = false;
      bool add_255_to_h_min = false;
      bool add_255_to_v_max = false;
      bool add_255_to_v_min = false;
      pos += BASE_DISPLAY_DESCRIPTOR_HEADER_SIZE - 1;  // Only for Display Range Limits Descriptor
      if (*(start + pos) >> 2 & BITMASK_TRUE(2) == 0b10) {
        add_255_to_h_max = true;
      } else if (*(start + pos) >> 2 & BITMASK_TRUE(2) == 0b11) {
        add_255_to_h_max = true;
        add_255_to_h_min = true;
      }
      if (*(start + pos) & BITMASK_TRUE(2) == 0b10) {
        add_255_to_v_max = true;
      } else if (*(start + pos) & BITMASK_TRUE(2) == 0b11) {
        add_255_to_v_max = true;
        add_255_to_v_min = true;
      }
      result.min_v_rate_hz = *(start + ++pos);
      result.max_v_rate_hz = *(start + ++pos);
      result.min_h_rate_khz = *(start + ++pos);
      result.max_h_rate_khz = *(start + ++pos);
      if (add_255_to_v_min)
        result.min_v_rate_hz += 255;
      if (add_255_to_v_max)
        result.max_v_rate_hz += 255;
      if (add_255_to_h_min)
        result.min_h_rate_khz += 255;
      if (add_255_to_h_max)
        result.max_h_rate_khz += 255;
      result.max_pixel_clock_rate_mhz = *(start + ++pos) * 10;
      result.video_timing_support = VideoTimingSupport(*(start + ++pos));
      return std::make_unique<DisplayRangeLimits>(result);
    }
  };

#define FIELDS(X) X.min_v_rate_hz, X.max_v_rate_hz, X.min_h_rate_khz, X.max_h_rate_khz, \
                  X.max_pixel_clock_rate_mhz, X.video_timing_support
  TIED_COMPARISONS(DisplayRangeLimits, FIELDS)
#undef FIELDS

  enum AsciiStringType {
    ASCII_DISPLAY_NAME = 0xFC,
    ASCII_UNSPECIFIED_TEXT = 0xFE,
    ASCII_SERIAL_NUMBER = 0xFF
  };

  STRINGIFY_ENUM(AsciiStringType, {
    {ASCII_DISPLAY_NAME,      "Display Name"},
    {ASCII_UNSPECIFIED_TEXT,  "Unspecified Text"},
    {ASCII_SERIAL_NUMBER,     "Serial Number"}
  })

  struct AsciiString : IEighteenByteDescriptor {
    std::string string;  // The maximum is 13 chars
    AsciiStringType descriptor_type = AsciiStringType::ASCII_UNSPECIFIED_TEXT;

    AsciiString() = default;

    // for brace-enclosed initialization despite
    // inheritance from a base class with virtual funcs
    AsciiString(
      const std::string& string_,
      AsciiStringType descriptor_type_
    )
      : string(string_)
      , descriptor_type(descriptor_type_)
    {}

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AsciiString, string, descriptor_type)
#endif

    uint8_t type() const override {
      return descriptor_type;
    }

    std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    static std::shared_ptr<AsciiString> parse_byte_block(const uint8_t* start) {
      AsciiString result;
      start += 3;
      result.descriptor_type = AsciiStringType(*start);
      start += 2;

      for (int i = 0; i < MAX_ASCII_STRING_LENGTH; ++i) {
        if (*(start + i) != '\n')
          result.string.push_back(*(start + i));
        else
          break;
      }
      return std::make_unique<AsciiString>(result);
    };
  };

#define FIELDS(X) X.string, X.descriptor_type
  TIED_COMPARISONS(AsciiString, FIELDS)
#undef FIELDS

  struct DummyDescriptor : IEighteenByteDescriptor {
    std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    uint8_t type() const override {
      return BASE_DISPLAY_DESCRIPTOR_DUMMY_TYPE;
    }
  };

  inline bool operator==(const DummyDescriptor& lhs, const DummyDescriptor& rhs) {
    return true;
  }

  struct EstablishedTimings3 : IEighteenByteDescriptor {
    std::array<EstablishedTimings, 6> bytes_6_11{};

    std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    uint8_t type() const override{
      return BASE_DISPLAY_DESCRIPTOR_ESTABLISHED_TIMINGS_III_TYPE;
    }

    static std::shared_ptr<EstablishedTimings3> parse_byte_block(const uint8_t* start) {
      EstablishedTimings3 result;
      start += BASE_DISPLAY_DESCRIPTOR_HEADER_SIZE;
      start++;

      for (int i = 0; i < 6; ++i) {
        result.bytes_6_11[i] = *(start + i);
      }
      return std::make_unique<EstablishedTimings3>(result);
    };
  };

#define FIELDS(X) X.bytes_6_11
  TIED_COMPARISONS(EstablishedTimings3, FIELDS)
#undef FIELDS

  struct ManufactureDate {
    uint8_t week_of_manufacture;
    uint16_t year_of_manufacture;  // This value becomes uint8_t in EDID by subtracting 1990 from it

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ManufactureDate,
      week_of_manufacture,
      year_of_manufacture
    )
#endif
  };

#define FIELDS(X) X.week_of_manufacture, X.year_of_manufacture
  TIED_COMPARISONS(ManufactureDate, FIELDS)
#undef FIELDS

  struct ModelYear {
    uint16_t model_year;  // This value becomes uint8_t in EDID by subtracting 1990 from it

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ModelYear,
      model_year
    )
#endif
  };

#define FIELDS(X) X.model_year
  TIED_COMPARISONS(ModelYear, FIELDS)
#undef FIELDS


  struct EighteenByteDescriptor {
    std::shared_ptr<IEighteenByteDescriptor> ptr;

    EighteenByteDescriptor()
      : ptr(std::make_shared<DummyDescriptor>())
    {}

    EighteenByteDescriptor(std::shared_ptr<IEighteenByteDescriptor> p)
      : ptr(p)
    {}
  };

  // See https://en.wikipedia.org/wiki/Extended_Display_Identification_Data
  struct BaseBlock {
    // Header
    std::array<char, 3> manufacturer_id;
    uint16_t product_code = 0;
    uint32_t serial_number = 0;
    std::variant<ManufactureDate, ModelYear> manufacture_date_or_model_year = ManufactureDate{0, 1990};
    uint8_t edid_major_version = 1;
    uint8_t edid_minor_version = 4;
    // Basic Display Parameters
    // NOTE: Only Digital Input supported
    BitDepth bits_per_color = BitDepth::BD_UNDEFINED;
    VideoInterface video_interface = VideoInterface::VI_UNDEFINED;
    uint8_t h_screen_size = 0;        // Horizontal screen size in cm - usually just choose values to set aspect ratio
    uint8_t v_screen_size = 0;
    uint8_t gamma = 0;
    bool dpms_standby = false;
    bool dpms_suspend = false;
    bool dpms_active_off = false;
    DigitalDisplayType display_type = DigitalDisplayType::DDT_RGB444;
    bool standard_srgb = false;
    bool preferred_timing_mode = true;
    bool continuous_timings = false;

    std::array<uint8_t, 10> chromaticity;

    // Established Timings
    EstablishedTimings established_timings_1 = 0x0;
    EstablishedTimings established_timings_2 = 0x0;
    EstablishedTimings manufacturers_timings = 0x0;

    std::array<std::optional<StandardTiming>, 8> standard_timings;
    std::array<EighteenByteDescriptor, BASE_18_BYTE_DESCRIPTORS> eighteen_byte_descriptors;

    BaseBlock() {
      manufacturer_id.fill('A');
      chromaticity.fill(0);
    }
  };

  bool operator==(const BaseBlock& lhs, const BaseBlock& rhs);

  /** Generates EDID Base Block binary */
  std::array<uint8_t, EDID_BLOCK_SIZE> generate_base_block(
    const BaseBlock& base_block,  /**< Base Block structure */
    uint8_t ext_blocks  /**< Number of extension blocks following the Base Block in effective EDID binary */
  );

  std::optional<StandardTiming> parse_standard_timing(uint8_t byte_1, uint8_t byte_2);
  std::pair<uint8_t, uint8_t> generate_standard_timing(const std::optional<StandardTiming>& std_timing);

  /** Parses EDID Base Block binary into Base Block structure and number of extension blocks in EDID binary */
  std::pair<BaseBlock, uint8_t> parse_base_block(
    const std::array<uint8_t, EDID_BLOCK_SIZE>& base_block  /**< EDID Base Block binary */
  );

  void print_standard_timing(
    std::ostream& os,  /**< Output stream */
    const StandardTiming& std_timing  /**< Standard Timing structure */
  );

  /** Prints Base Block structure into stream */
  void print_base_block(
    std::ostream& os,  /**< Output stream */
    const BaseBlock& base_block  /**< Base Block structure */
  );
}  // namespace Edid
