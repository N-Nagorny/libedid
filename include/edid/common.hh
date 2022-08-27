#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <variant>

#include "eighteen_byte_descriptor.hh"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#define BITMASK_TRUE(length) static_cast<uint8_t>(std::pow(2.0, length) - 1)

#define STRINGIFY_ENUM(ENUM_TYPE, ...)                                             \
  inline std::string to_string(const ENUM_TYPE& e)                                 \
  {                                                                                \
    static_assert(std::is_enum<ENUM_TYPE>::value, #ENUM_TYPE " must be an enum!"); \
    static const std::pair<ENUM_TYPE, std::string> m[] = __VA_ARGS__;              \
    auto it = std::find_if(std::begin(m), std::end(m),                             \
      [e](const std::pair<ENUM_TYPE, std::string>& es_pair) -> bool                \
    {                                                                              \
      return es_pair.first == e;                                                   \
    });                                                                            \
    return ((it != std::end(m)) ? it : std::begin(m))->second;                     \
  }                                                                                \

#define EDID_BLOCK_SIZE 128
#define NTSC_FACTOR_NUMERATOR 1000
#define NTSC_FACTOR_DENOMINATOR 1001

namespace Edid {
  enum StereoMode {
    NO_STEREO = 0b000,
    FIELD_SEQUENTIAL_L_R = 0b010,
    FIELD_SEQUENTIAL_R_L = 0b100,
    INTERLEAVED_RIGHT_EVEN = 0b011,
    INTERLEAVED_LEFT_EVEN = 0b101,
    FOUR_WAY_INTERLEAVED = 0b110,
    SIDE_BY_SIDE_INTERLEAVED = 0b111
  };

  struct AnalogCompositeSync {
    bool bipolar;
    bool serrations;
    bool sync_on_rgb_signals;
  };

  namespace details {
    template<typename T>
    bool operator!=(const T& lhs, const T& rhs) {
      return !(lhs == rhs);
    }
  }

  bool operator==(const AnalogCompositeSync& lhs, const AnalogCompositeSync& rhs);

  struct DigitalCompositeSync {
    bool serrations;
    bool h_sync_polarity;
  };

  bool operator==(const DigitalCompositeSync& lhs, const DigitalCompositeSync& rhs);

  struct DigitalSeparateSync {
    bool v_sync_polarity;
    bool h_sync_polarity;
  };

  bool operator==(const DigitalSeparateSync& lhs, const DigitalSeparateSync& rhs);

  struct DtdFeaturesBitmap {
    bool interlaced;
    StereoMode stereo_mode;
    std::variant<AnalogCompositeSync, DigitalCompositeSync, DigitalSeparateSync> sync;
  };

  bool operator==(const DtdFeaturesBitmap& lhs, const DtdFeaturesBitmap& rhs);

  struct DetailedTimingDescriptor {
    uint64_t pixel_clock_hz; // This value becomes uint16_t in EDID by dividing by 10'000
    uint16_t h_res;
    uint16_t v_res;
    uint16_t h_blank_pixels;
    uint16_t v_blank_lines;
    uint16_t h_sync_offset; // Horizontal front porch in other words
    uint16_t h_sync_width;
    uint8_t v_sync_offset; // Vertical front porch in other words
    uint8_t v_sync_width;
    uint16_t h_image_size;
    uint16_t v_image_size;
    uint8_t h_border_pixels;
    uint8_t v_border_lines;
    DtdFeaturesBitmap features_bitmap;

    std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const;
    void print(std::ostream& os, uint8_t tabs = 1) const;
    static DetailedTimingDescriptor parse_byte_block(const std::array<uint8_t, EIGHTEEN_BYTES>& block);

    uint8_t type() const {
      return BASE_FAKE_DTD_TYPE;
    }
  };

  bool operator==(const DetailedTimingDescriptor& lhs, const DetailedTimingDescriptor& rhs);

  template<typename E>
  std::vector<E> bitfield_to_enums(uint8_t bitfield) {
    static_assert(std::is_enum<E>::value,
      "bitfield_to_enums: template parameter must be an enum!");
    std::vector<E> result;
    for (int i = 0; i < 8; ++i)
      if (bitfield & (1 << i))
        result.push_back(static_cast<E>(1 << i));
    return result;
  }
  uint8_t calculate_block_checksum(const std::array<uint8_t, EDID_BLOCK_SIZE>& block);
}
