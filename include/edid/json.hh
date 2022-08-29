#pragma once

#include <nlohmann/json.hpp>

#include "base_block.hh"
#include "common.hh"
#include "cta861_block.hh"

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

  void from_json(const nlohmann::json& j, UnknownDataBlock&);
  void from_json(const nlohmann::json& j, VideoDataBlock&);
  void from_json(const nlohmann::json& j, AudioDataBlock&);
  void from_json(const nlohmann::json& j, SpeakerAllocationDataBlock&);
  void from_json(const nlohmann::json& j, Cta861Block&);
  void from_json(const nlohmann::json& j, ShortAudioDescriptor& result);

  void to_json(nlohmann::json& j, const UnknownDataBlock&);
  void to_json(nlohmann::json& j, const ShortAudioDescriptor&);
  void to_json(nlohmann::json& j, const VideoDataBlock&);
  void to_json(nlohmann::json& j, const AudioDataBlock&);
  void to_json(nlohmann::json& j, const SpeakerAllocationDataBlock&);
  void to_json(nlohmann::json& j, const Cta861Block&);
}