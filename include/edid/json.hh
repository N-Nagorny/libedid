#pragma once

#include <nlohmann/json.hpp>

#include "base_block.hh"
#include "common.hh"

namespace Edid {

  NLOHMANN_JSON_SERIALIZE_ENUM(StereoMode, {
    {NO_STEREO, "NO_STEREO"},
    {FIELD_SEQUENTIAL_L_R, "FIELD_SEQUENTIAL_L_R"},
    {FIELD_SEQUENTIAL_R_L, "FIELD_SEQUENTIAL_R_L"},
    {INTERLEAVED_RIGHT_EVEN, "INTERLEAVED_RIGHT_EVEN"},
    {INTERLEAVED_LEFT_EVEN, "INTERLEAVED_LEFT_EVEN"},
    {FOUR_WAY_INTERLEAVED, "FOUR_WAY_INTERLEAVED"},
    {SIDE_BY_SIDE_INTERLEAVED, "SIDE_BY_SIDE_INTERLEAVED"}
  })

  void from_json(const nlohmann::json& j, BaseBlock&);
  void from_json(const nlohmann::json& j, DetailedTimingDescriptor&);

  void to_json(nlohmann::json& j, const BaseBlock&);
  void to_json(nlohmann::json& j, const DetailedTimingDescriptor&);
}
