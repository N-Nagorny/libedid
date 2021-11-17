#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

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
#define DTD_BLOCK_SIZE 18

namespace Edid {
  struct DetailedTimingDescriptor {
    uint32_t pixel_clock_hz; // This value becomes uint16_t in EDID by dividing by 10'000
    uint16_t h_res;
    uint16_t v_res;
    uint16_t h_blank_pixels;
    uint16_t v_blank_lines;
    uint16_t h_sync_offset;
    uint16_t h_sync_width;
    uint8_t v_sync_offset;
    uint8_t v_sync_width;
    uint16_t h_image_size;
    uint16_t v_image_size;
    uint8_t h_border_pixels;
    uint8_t v_border_lines;
    uint8_t features_bitmap;

    uint8_t size() const {
      return DTD_BLOCK_SIZE;
    }
  };

  bool operator==(const DetailedTimingDescriptor& lhs, const DetailedTimingDescriptor& rhs);
  bool operator!=(const DetailedTimingDescriptor& lhs, const DetailedTimingDescriptor& rhs);

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

  std::array<uint8_t, DTD_BLOCK_SIZE> make_dtd(const DetailedTimingDescriptor& dtd);
  uint8_t calculate_block_checksum(const std::array<uint8_t, EDID_BLOCK_SIZE>& block);
}
