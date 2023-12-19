// Copyright 2023 N-Nagorny
#pragma once

#include <nlohmann/json.hpp>

#include "edid.hh"

namespace std {
  template<typename T>
  void from_json(const nlohmann::json& j, std::optional<T>& opt) {
    if (j.is_null()) {
      opt = std::nullopt;
    }
    else {
      opt = j.get<T>();
    }
  }

  template<typename T>
  void to_json(nlohmann::json& j, const std::optional<T>& opt) {
    if (opt.has_value()) {
      j = *opt;
    }
    else {
      j = nullptr;
    }
  }
}  // namespace std

namespace Edid {
  // Base Block

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StandardTiming, x_resolution, aspect_ratio, v_frequency)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ManufactureDate,
    week_of_manufacture,
    year_of_manufacture
  )
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ModelYear,
    model_year
  )

  void from_json(const nlohmann::json& j, BaseBlock&);
  void from_json(const nlohmann::json& j, DetailedTimingDescriptor&);
  void from_json(const nlohmann::json& j, EstablishedTimings3&);
  void from_json(const nlohmann::json& j, EdidData&);

  void to_json(nlohmann::json& j, const BaseBlock&);
  void to_json(nlohmann::json& j, const DetailedTimingDescriptor&);
  void to_json(nlohmann::json& j, const EstablishedTimings3&);
  void to_json(nlohmann::json& j, const EdidData&);

  // DTD

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnalogCompositeSync, bipolar, serrations, sync_on_rgb_signals)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DigitalCompositeSync, serrations, h_sync_polarity)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DigitalSeparateSync, v_sync_polarity, h_sync_polarity)

  // CTA Data Block

  void from_json(const nlohmann::json&, ColorimetryDataBlock&);
  void from_json(const nlohmann::json&, Cta861Block&);
  void from_json(const nlohmann::json&, HdrStaticMetadataDataBlock&);
  void from_json(const nlohmann::json&, ShortAudioDescriptor&);
  void from_json(const nlohmann::json&, SpeakerAllocationDataBlock&);
  void from_json(const nlohmann::json&, YCbCr420CapabilityMapDataBlock&);

  void to_json(nlohmann::json&, const ColorimetryDataBlock&);
  void to_json(nlohmann::json&, const Cta861Block&);
  void to_json(nlohmann::json&, const HdrStaticMetadataDataBlock&);
  void to_json(nlohmann::json&, const ShortAudioDescriptor&);
  void to_json(nlohmann::json&, const SpeakerAllocationDataBlock&);
  void to_json(nlohmann::json&, const YCbCr420CapabilityMapDataBlock&);

  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VideoDataBlock, vics)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioDataBlock, sads)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UnknownDataBlock, raw_data, data_block_tag, extended_tag)
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VideoCapabilityDataBlock, is_ycc_quantization_range_selectable,
    is_rgb_quantization_range_selectable, pt_scan_behaviour, it_scan_behaviour, ce_scan_behaviour)

  // HDMI VSDB

  void from_json(const nlohmann::json& j, Vic3dSupport&);
  void from_json(const nlohmann::json& j, StereoVideoSupport&);
  void from_json(const nlohmann::json& j, HdmiVideoSubblock&);
  void from_json(const nlohmann::json& j, HdmiVendorDataBlock& result);

  void to_json(nlohmann::json& j, const Vic3dSupport&);
  void to_json(nlohmann::json& j, const StereoVideoSupport&);
  void to_json(nlohmann::json& j, const HdmiVideoSubblock&);
  void to_json(nlohmann::json& j, const HdmiVendorDataBlock& block);
}  // namespace Edid
