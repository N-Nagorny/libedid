// Copyright 2023 N-Nagorny
#pragma once

#include <nlohmann/json.hpp>

#include "edid.hh"

namespace Edid {
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StandardTiming, x_resolution, aspect_ratio, v_frequency)

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ManufactureDate,
    week_of_manufacture,
    year_of_manufacture
  )

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ModelYear,
    model_year
  )

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnalogCompositeSync, bipolar, serrations, sync_on_rgb_signals)

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DigitalCompositeSync, serrations, h_sync_polarity)

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DigitalSeparateSync, v_sync_polarity, h_sync_polarity)

  void from_json(const nlohmann::json& j, BaseBlock&);
  void from_json(const nlohmann::json& j, DetailedTimingDescriptor&);
  void from_json(const nlohmann::json& j, EstablishedTimings3&);
  void from_json(const nlohmann::json& j, EdidData&);

  void to_json(nlohmann::json& j, const BaseBlock&);
  void to_json(nlohmann::json& j, const DetailedTimingDescriptor&);
  void to_json(nlohmann::json& j, const EstablishedTimings3&);
  void to_json(nlohmann::json& j, const EdidData&);

  void from_json(const nlohmann::json& j, UnknownDataBlock&);
  void from_json(const nlohmann::json& j, VideoDataBlock&);
  void from_json(const nlohmann::json& j, AudioDataBlock&);
  void from_json(const nlohmann::json& j, SpeakerAllocationDataBlock&);
  void from_json(const nlohmann::json& j, YCbCr420CapabilityMapDataBlock&);
  void from_json(const nlohmann::json& j, ColorimetryDataBlock&);
  void from_json(const nlohmann::json& j, Cta861Block&);
  void from_json(const nlohmann::json& j, ShortAudioDescriptor& result);

  void to_json(nlohmann::json& j, const UnknownDataBlock&);
  void to_json(nlohmann::json& j, const ShortAudioDescriptor&);
  void to_json(nlohmann::json& j, const VideoDataBlock&);
  void to_json(nlohmann::json& j, const AudioDataBlock&);
  void to_json(nlohmann::json& j, const SpeakerAllocationDataBlock&);
  void to_json(nlohmann::json& j, const YCbCr420CapabilityMapDataBlock&);
  void to_json(nlohmann::json& j, const ColorimetryDataBlock&);
  void to_json(nlohmann::json& j, const Cta861Block&);

  void from_json(const nlohmann::json& j, Vic3dSupport&);
  void from_json(const nlohmann::json& j, StereoVideoSupport&);
  void from_json(const nlohmann::json& j, HdmiVideoSubblock&);
  void from_json(const nlohmann::json& j, HdmiVendorDataBlock& result);

  void to_json(nlohmann::json& j, const Vic3dSupport&);
  void to_json(nlohmann::json& j, const StereoVideoSupport&);
  void to_json(nlohmann::json& j, const HdmiVideoSubblock&);
  void to_json(nlohmann::json& j, const HdmiVendorDataBlock& block);
}  // namespace Edid
