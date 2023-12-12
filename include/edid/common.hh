// Copyright 2023 N-Nagorny

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#ifdef ENABLE_JSON
#include <nlohmann/json.hpp>
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#define BITMASK_TRUE(length) static_cast<uint8_t>(std::pow(2.0, length) - 1)

#ifdef ENABLE_JSON
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
  }
#else
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
  }
#endif

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
