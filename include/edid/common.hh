// Copyright 2023 N-Nagorny
#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <nlohmann/json.hpp>

#include "eighteen_byte_descriptor.hh"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#define BITMASK_TRUE(length) static_cast<uint8_t>(std::pow(2.0, length) - 1)

#define STRINGIFY_ENUM(ENUM_TYPE, ...)                                             \
  NLOHMANN_JSON_SERIALIZE_ENUM(ENUM_TYPE, __VA_ARGS__)                             \
                                                                                   \
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


#define TIED_OP(STRUCT, OP, GET_FIELDS)                                            \
    inline bool operator OP(const STRUCT& lhs, const STRUCT& rhs)                  \
    {                                                                              \
        return std::tie(GET_FIELDS(lhs)) OP std::tie(GET_FIELDS(rhs));             \
    }                                                                              \


#define TIED_COMPARISONS(STRUCT, GET_FIELDS)                                       \
    TIED_OP(STRUCT, ==, GET_FIELDS)                                                \
    TIED_OP(STRUCT, !=, GET_FIELDS)                                                \
    TIED_OP(STRUCT, <, GET_FIELDS)                                                 \
    TIED_OP(STRUCT, <=, GET_FIELDS)                                                \
    TIED_OP(STRUCT, >=, GET_FIELDS)                                                \
    TIED_OP(STRUCT, >, GET_FIELDS)                                                 \


#define EDID_BLOCK_SIZE 128
#define NTSC_FACTOR_NUMERATOR 1000
#define NTSC_FACTOR_DENOMINATOR 1001

#define ENUM_NULL 0

namespace Edid {
  // See Overload Pattern on
  // https://www.modernescpp.com/index.php/visiting-a-std-variant-with-the-overload-pattern/
  template<typename ... Ts>
  struct Overload : Ts ... {
      using Ts::operator() ...;
  };
  template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

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

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnalogCompositeSync, bipolar, serrations, sync_on_rgb_signals)
#endif
  };

#define FIELDS(X) X.bipolar, X.serrations, X.sync_on_rgb_signals
  TIED_COMPARISONS(AnalogCompositeSync, FIELDS)
#undef FIELDS

  struct DigitalCompositeSync {
    bool serrations;
    bool h_sync_polarity;

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DigitalCompositeSync, serrations, h_sync_polarity)
#endif
  };

#define FIELDS(X) X.serrations, X.h_sync_polarity
  TIED_COMPARISONS(DigitalCompositeSync, FIELDS)
#undef FIELDS

  struct DigitalSeparateSync {
    bool v_sync_polarity;
    bool h_sync_polarity;

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DigitalSeparateSync, v_sync_polarity, h_sync_polarity)
#endif
  };

#define FIELDS(X) X.v_sync_polarity, X.h_sync_polarity
  TIED_COMPARISONS(DigitalSeparateSync, FIELDS)
#undef FIELDS

  struct DtdFeaturesBitmap {
    bool interlaced;
    StereoMode stereo_mode;
    std::variant<AnalogCompositeSync, DigitalCompositeSync, DigitalSeparateSync> sync;
  };

#define FIELDS(X) X.interlaced, X.stereo_mode, X.sync
  TIED_COMPARISONS(DtdFeaturesBitmap, FIELDS)
#undef FIELDS

  struct DetailedTimingDescriptor {
    uint64_t pixel_clock_hz;  // This value becomes uint16_t in EDID by dividing by 10'000
    uint16_t h_res;
    uint16_t v_res;
    uint16_t h_blanking;
    uint16_t v_blanking;
    uint16_t h_front_porch;
    uint16_t h_sync_width;
    uint8_t v_front_porch;
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

#define FIELDS(X) X.pixel_clock_hz, X.h_res, X.v_res, X.h_blanking, X.v_blanking,      \
                  X.h_front_porch, X.h_sync_width, X.v_front_porch, X.v_sync_width,    \
                  X.h_image_size, X.v_image_size, X.h_border_pixels, X.v_border_lines, \
                  X.features_bitmap
  TIED_COMPARISONS(DetailedTimingDescriptor, FIELDS)
#undef FIELDS

  template<typename E, typename T>
  std::vector<E> bitfield_to_enums(T bitfield) {
    static_assert(std::is_enum<E>::value,
      "bitfield_to_enums: Template parameter T must be an enum!");
    static_assert(std::is_arithmetic<T>::value,
      "bitfield_to_enums: Template parameter E must be an arithmetic type!");
    std::vector<E> result;
    for (int i = 0; i < 8 * sizeof(T); ++i)
      if (bitfield & (1 << i))
        result.push_back(static_cast<E>(1 << i));
    return result;
  }
  uint8_t calculate_block_checksum(const std::array<uint8_t, EDID_BLOCK_SIZE>& block);
  std::vector<uint8_t> read_file(const std::string& file_path);
}  // namespace Edid
