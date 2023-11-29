// Copyright 2023 N-Nagorny
#include "edid/json.hh"

namespace Edid {
  void from_json(const nlohmann::json& j, std::variant<AnalogCompositeSync, DigitalCompositeSync, DigitalSeparateSync>& sync) {
    if (j.contains("bipolar")) {
      AnalogCompositeSync subresult;
      from_json(j, subresult);
      sync = subresult;
    }
    else if (j.contains("v_sync_polarity")) {
      DigitalSeparateSync subresult;
      from_json(j, subresult);
      sync = subresult;
    }
    else {
      DigitalCompositeSync subresult;
      from_json(j, subresult);
      sync = subresult;
    }
  }

  void from_json(const nlohmann::json& j, std::variant<ManufactureDate, ModelYear>& manufacture_date_or_model_year) {
    if (j.contains("model_year")) {
      ModelYear subresult;
      from_json(j, subresult);
      manufacture_date_or_model_year = subresult;
    }
    else {
      ManufactureDate subresult;
      from_json(j, subresult);
      manufacture_date_or_model_year = subresult;
    }
  }

  void from_json(const nlohmann::json& j, DetailedTimingDescriptor& dtd) {
    DetailedTimingDescriptor result;

    result.pixel_clock_hz = j.at("pixel_clock_khz").get<uint64_t>() * 1'000;
    result.h_res = j.at("h_res");
    result.v_res = j.at("v_res");
    result.h_blanking = j.at("h_blanking");
    result.v_blanking = j.at("v_blanking");
    result.h_front_porch = j.at("h_front_porch");
    result.v_front_porch = j.at("v_front_porch");
    result.h_sync_width = j.at("h_sync_width");
    result.v_sync_width = j.at("v_sync_width");
    result.h_image_size = j.at("h_image_size");
    result.v_image_size = j.at("v_image_size");
    result.h_border_pixels = j.at("h_border");
    result.v_border_lines = j.at("v_border");
    result.features_bitmap = DtdFeaturesBitmap{
      j.at("interlaced"),
      StereoMode(j.at("stereo_mode")),
      j.at("sync")
    };

    dtd = std::move(result);
  }

  void to_json(nlohmann::json& j, const DetailedTimingDescriptor& dtd) {
    nlohmann::json result;
    result["pixel_clock_khz"] = dtd.pixel_clock_hz / 1'000;
    result["h_res"] = dtd.h_res;
    result["v_res"] = dtd.v_res;
    result["h_blanking"] = dtd.h_blanking;
    result["v_blanking"] = dtd.v_blanking;
    result["h_front_porch"] = dtd.h_front_porch;
    result["v_front_porch"] = dtd.v_front_porch;
    result["h_sync_width"] = dtd.h_sync_width;
    result["v_sync_width"] = dtd.v_sync_width;
    result["h_image_size"] = dtd.h_image_size;
    result["v_image_size"] = dtd.v_image_size;
    result["h_border"] = dtd.h_border_pixels;
    result["v_border"] = dtd.v_border_lines;
    result["interlaced"] = dtd.features_bitmap.interlaced;
    result["stereo_mode"] = dtd.features_bitmap.stereo_mode;
    result["sync"] = std::visit([](const auto& sync) -> nlohmann::json {
      nlohmann::json result;
      to_json(result, sync);
      return result;
    }, dtd.features_bitmap.sync);
    j = std::move(result);
  }

  void from_json(const nlohmann::json& j, EighteenByteDescriptor& descriptor) {
    if (j.contains("pixel_clock_khz")) {
      DetailedTimingDescriptor subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
    else if (j.contains("descriptor_type")) {
      AsciiString subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
    else if (j.contains("established_timings_3")) {
      EstablishedTimings3 subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
    else if (j.contains("min_v_rate_hz")) {
      DisplayRangeLimits subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
  }

  void from_json(const nlohmann::json& j, VideoDataBlock& result) {
    auto vics = j.at("vics").get<std::vector<uint8_t>>();
    for (int i = 0; i < vics.size(); ++i) {
      result.vics[i] = vics.at(i);
    }
  }

  void to_json(nlohmann::json& j, const VideoDataBlock& block) {
    for (const auto& vic : block.vics) {
      if (vic.has_value()) {
        j["vics"].push_back(vic.value());
      }
    }
  }

  void from_json(const nlohmann::json& j, SpeakerAllocationDataBlock& result) {
    auto speakers = j.at("speaker_allocation").get<std::vector<Speaker>>();
    for (auto speaker : speakers) {
      result.speaker_allocation |= speaker;
    }
  }

  void to_json(nlohmann::json& j, const SpeakerAllocationDataBlock& block) {
    for (Speaker speaker : bitfield_to_enums<Speaker>(block.speaker_allocation))
      j["speaker_allocation"].push_back(to_string(speaker));
  }

  void from_json(const nlohmann::json& j, YCbCr420CapabilityMapDataBlock& result) {
    auto svd_indices = j.at("ycbcr420_capability_map").get<std::vector<uint8_t>>();
    result.svd_indices = std::set<uint8_t>(svd_indices.begin(), svd_indices.end());
  }

  void to_json(nlohmann::json& j, const YCbCr420CapabilityMapDataBlock& block) {
    for (uint8_t svd_index : block.svd_indices)
      j["ycbcr420_capability_map"].push_back(svd_index);
  }

  void from_json(const nlohmann::json& j, ColorimetryDataBlock& result) {
    const auto standards = j.at("colorimetry_standards").get<std::vector<ColorimetryStandard>>();
    const auto profiles = j.at("gamut_metadata_profiles").get<std::vector<GamutMetadataProfile>>();

    for (const auto& standard : standards) {
      result.colorimetry_standards |= standard;
    }
    for (const auto& profile : profiles) {
      result.gamut_metadata_profiles |= profile;
    }
  }

  void to_json(nlohmann::json& j, const ColorimetryDataBlock& block) {
    j = {
      {"colorimetry_standards",   nlohmann::json::array()},
      {"gamut_metadata_profiles", nlohmann::json::array()}
    };

    for (ColorimetryStandard standard : bitfield_to_enums<ColorimetryStandard>(block.colorimetry_standards)) {
      j["colorimetry_standards"].push_back(to_string(standard));
    }
    for (GamutMetadataProfile profile : bitfield_to_enums<GamutMetadataProfile>(block.gamut_metadata_profiles)) {
      j["gamut_metadata_profiles"].push_back(to_string(profile));
    }
  }

  void from_json(const nlohmann::json& j, UnknownDataBlock& result) {
    result.raw_data = j.at("raw_data").get<std::vector<uint8_t>>();
    result.data_block_tag = j.at("tag");
    if (j.contains("extended_tag")) {
      result.extended_tag = j.at("extended_tag").get<uint8_t>();
    }
  }

  void to_json(nlohmann::json& j, const UnknownDataBlock& block) {
    j["raw_data"] = block.raw_data;
    j["tag"] = block.data_block_tag;
    if (block.extended_tag.has_value())
      j["extended_tag"] = block.extended_tag.value();
  }

  void from_json(const nlohmann::json& j, ShortAudioDescriptor& result) {
    result.audio_format = j.at("audio_format");
    result.channels = AudioChannels(j.at("channels").get<uint8_t>() - 1);
    auto freqs = j.at("sampling_freqs").get<std::vector<SamplingFrequence>>();
    for (auto freq : freqs) {
      result.sampling_freqs |= freq;
    }
    if (j.contains("lpcm_bit_depths")) {
      auto depths = j.at("lpcm_bit_depths").get<std::vector<uint8_t>>();
      for (auto depth : depths) {
        switch (depth) {
          case 24:
            result.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_24;
            break;
          case 20:
            result.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_20;
            break;
          case 16:
            result.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_16;
            break;
        }
      }
    }
  }

  void to_json(nlohmann::json& j, const ShortAudioDescriptor& block) {
    j["audio_format"] = block.audio_format;
    j["channels"] = block.channels + 1;
    for (SamplingFrequence freq : bitfield_to_enums<SamplingFrequence>(block.sampling_freqs))
      j["sampling_freqs"].push_back(to_string(freq));
    for (LpcmBitDepth depth : bitfield_to_enums<LpcmBitDepth>(block.lpcm_bit_depths)) {
      switch (depth) {
        case LPCM_BD_24:
          j["lpcm_bit_depths"].push_back(24);
          break;
        case LPCM_BD_20:
          j["lpcm_bit_depths"].push_back(20);
          break;
        case LPCM_BD_16:
          j["lpcm_bit_depths"].push_back(16);
          break;
      }
    }
  }

  void from_json(const nlohmann::json& j, AudioDataBlock& result) {
    auto sads = j.at("sads").get<std::vector<ShortAudioDescriptor>>();
    for (int i = 0; i < sads.size(); ++i) {
      result.sads[i] = sads.at(i);
    }
  }

  void to_json(nlohmann::json& j, const AudioDataBlock& block) {
    for (const auto& sad : block.sads) {
      if (sad.has_value()) {
        j["sads"].push_back(sad.value());
      }
    }
  }

  void from_json(const nlohmann::json& j, EstablishedTimings3& established_timings_3_) {
    EstablishedTimings3 result;

    const auto& established_timings_3 = j.at("established_timings_3").get<std::vector<nlohmann::json>>();
    for (const auto& established_timing : established_timings_3) {
      EstablishedTiming3Byte6 byte_6 = established_timing;
      EstablishedTiming3Byte7 byte_7 = established_timing;
      EstablishedTiming3Byte8 byte_8 = established_timing;
      EstablishedTiming3Byte9 byte_9 = established_timing;
      EstablishedTiming3Byte10 byte_10 = established_timing;
      EstablishedTiming3Byte11 byte_11 = established_timing;

      if (byte_6 != ENUM_NULL) {
        result.bytes_6_11[0] |= byte_6;
      }
      else if (byte_7 != ENUM_NULL) {
        result.bytes_6_11[1] |= byte_7;
      }
      else if (byte_8 != ENUM_NULL) {
        result.bytes_6_11[2] |= byte_8;
      }
      else if (byte_9 != ENUM_NULL) {
        result.bytes_6_11[3] |= byte_9;
      }
      else if (byte_10 != ENUM_NULL) {
        result.bytes_6_11[4] |= byte_10;
      }
      else if (byte_11 != ENUM_NULL) {
        result.bytes_6_11[5] |= byte_11;
      }
    }

    established_timings_3_ = std::move(result);
  }

  void to_json(nlohmann::json& j, const EstablishedTimings3& established_timings_3) {
    nlohmann::json result;

    const auto push_to_json_array = [&result] (const auto& et) {
      if (et != ENUM_NULL) {
        result["established_timings_3"].push_back(et);
      }
    };

    for (EstablishedTiming3Byte6 et : bitfield_to_enums<EstablishedTiming3Byte6>(established_timings_3.bytes_6_11.at(0)))
      push_to_json_array(et);
    for (EstablishedTiming3Byte7 et : bitfield_to_enums<EstablishedTiming3Byte7>(established_timings_3.bytes_6_11.at(1)))
      push_to_json_array(et);
    for (EstablishedTiming3Byte8 et : bitfield_to_enums<EstablishedTiming3Byte8>(established_timings_3.bytes_6_11.at(2)))
      push_to_json_array(et);
    for (EstablishedTiming3Byte9 et : bitfield_to_enums<EstablishedTiming3Byte9>(established_timings_3.bytes_6_11.at(3)))
      push_to_json_array(et);
    for (EstablishedTiming3Byte10 et : bitfield_to_enums<EstablishedTiming3Byte10>(established_timings_3.bytes_6_11.at(4)))
      push_to_json_array(et);
    for (EstablishedTiming3Byte11 et : bitfield_to_enums<EstablishedTiming3Byte11>(established_timings_3.bytes_6_11.at(5)))
      push_to_json_array(et);

    j = std::move(result);
  }

  void from_json(const nlohmann::json& j, BaseBlock& base_block) {
    BaseBlock result;

    for (int i = 0; i < 3; ++i)
      result.manufacturer_id[i] = j.at("manufacturer_id").get<std::string>().at(i);
    result.product_code = j.at("product_code");
    result.serial_number = j.at("serial_number");
    result.manufacture_date_or_model_year = j.at("manufacture_date_or_model_year");
    result.edid_major_version = j.at("edid_major_version");
    result.edid_minor_version = j.at("edid_minor_version");
    result.bits_per_color = BitDepth(j.at("bit_depth"));
    result.video_interface = VideoInterface(j.at("video_interface"));
    result.h_screen_size = j.at("h_screen_size");
    result.v_screen_size = j.at("v_screen_size");
    result.gamma = j.at("gamma");
    result.dpms_standby = j.at("dpms_standby");
    result.dpms_suspend = j.at("dpms_suspend");
    result.dpms_active_off = j.at("dpms_active_off");
    result.display_type = DigitalDisplayType(j.at("digital_display_type"));
    result.standard_srgb = j.at("srgb_is_primary");
    result.preferred_timing_mode = j.at("preferred_timing_mode");
    result.continuous_timings = j.at("continuous_timings");
    auto chromaticity = j.at("chromaticity").get<std::vector<uint8_t>>();
    for (int i = 0; i < chromaticity.size(); ++i) {
      result.chromaticity[i] = chromaticity.at(i);
    }
    auto et_1 = j.at("established_timings_1").get<std::vector<EstablishedTiming1>>();
    for (auto et : et_1) {
      result.established_timings_1 |= et;
    }
    auto et_2 = j.at("established_timings_2").get<std::vector<EstablishedTiming2>>();
    for (auto et : et_2) {
      result.established_timings_2 |= et;
    }
    auto et_3 = j.at("manufacturers_timings").get<std::vector<ManufacturersTiming>>();
    for (auto et : et_3) {
      result.manufacturers_timings |= et;
    }
    auto standard_timings = j.at("standard_timings").get<std::vector<StandardTiming>>();
    for (int i = 0; i < standard_timings.size(); ++i) {
      result.standard_timings[i] = standard_timings.at(i);
    }
    auto eighteen_byte_descriptors = j.at("eighteen_byte_descriptors").get<std::vector<EighteenByteDescriptor>>();
    for (int i = 0; i < eighteen_byte_descriptors.size(); ++i) {
      result.eighteen_byte_descriptors[i] = eighteen_byte_descriptors.at(i);
    }

    base_block = std::move(result);
  }

  void to_json(nlohmann::json& j, const BaseBlock& base_block) {
    nlohmann::json result;
    std::string manufacturer_id = {
      base_block.manufacturer_id.at(0),
      base_block.manufacturer_id.at(1),
      base_block.manufacturer_id.at(2)
    };
    result["manufacturer_id"] = manufacturer_id;
    result["product_code"] = base_block.product_code;
    result["serial_number"] = base_block.serial_number;
    result["manufacture_date_or_model_year"] = std::visit([](const auto& manufacture_date_or_model_year) -> nlohmann::json {
      nlohmann::json result;
      to_json(result, manufacture_date_or_model_year);
      return result;
    }, base_block.manufacture_date_or_model_year);
    result["edid_major_version"] = base_block.edid_major_version;
    result["edid_minor_version"] = base_block.edid_minor_version;
    result["bit_depth"] = to_string(base_block.bits_per_color);
    result["video_interface"] = to_string(base_block.video_interface);
    result["h_screen_size"] = base_block.h_screen_size;
    result["v_screen_size"] = base_block.v_screen_size;
    result["gamma"] = base_block.gamma;
    result["dpms_standby"] = base_block.dpms_standby;
    result["dpms_suspend"] = base_block.dpms_suspend;
    result["dpms_active_off"] = base_block.dpms_active_off;
    result["digital_display_type"] = to_string(base_block.display_type);
    result["srgb_is_primary"] = base_block.standard_srgb;
    result["preferred_timing_mode"] = base_block.preferred_timing_mode;
    result["continuous_timings"] = base_block.continuous_timings;
    for (const auto& byte : base_block.chromaticity) {
      result["chromaticity"].push_back(byte);
    }
    result["established_timings_1"] = nlohmann::json::array();
    result["established_timings_2"] = nlohmann::json::array();
    result["manufacturers_timings"] = nlohmann::json::array();
    for (EstablishedTiming1 et : bitfield_to_enums<EstablishedTiming1>(base_block.established_timings_1))
      result["established_timings_1"].push_back(to_string(et));
    for (EstablishedTiming2 et : bitfield_to_enums<EstablishedTiming2>(base_block.established_timings_2))
      result["established_timings_2"].push_back(to_string(et));
    for (ManufacturersTiming et : bitfield_to_enums<ManufacturersTiming>(base_block.manufacturers_timings))
      result["manufacturers_timings"].push_back(to_string(et));
    result["standard_timings"] = nlohmann::json::array();
    for (const auto& timing : base_block.standard_timings) {
      if (timing.has_value()) {
        result["standard_timings"].push_back(timing.value());
      }
    }
    auto eighteen_byte_descriptor_visitor = Overload {
      [](const DummyDescriptor&) -> std::optional<nlohmann::json> {
        return std::nullopt;
      },
      [](const auto& d) -> std::optional<nlohmann::json> {
        nlohmann::json result;
        to_json(result, d);
        return result;
      },
    };
    for (const auto& descriptor : base_block.eighteen_byte_descriptors) {
      if (descriptor.has_value()) {
        auto d_json = std::visit(eighteen_byte_descriptor_visitor, descriptor.value());
        if (d_json.has_value())
          result["eighteen_byte_descriptors"].push_back(d_json.value());
      }
    }
    j = std::move(result);
  }

  void from_json(const nlohmann::json& j, CtaDataBlock& descriptor) {
    if (j.contains("sads")) {
      AudioDataBlock subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
    else if (j.contains("vics")) {
      VideoDataBlock subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
    else if (j.contains("speaker_allocation")) {
      SpeakerAllocationDataBlock subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
    else if (j.contains("ycbcr420_capability_map")) {
      YCbCr420CapabilityMapDataBlock subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
    else if (j.contains("colorimetry_standards")) {
      ColorimetryDataBlock subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
    else if (j.contains("hdmi_vsdb")) {
      HdmiVendorDataBlock subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
    else if (j.contains("raw_data")) {
      UnknownDataBlock subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
  }

  void from_json(const nlohmann::json& j, Cta861Block& result) {
    result.underscan = j.at("underscan");
    result.basic_audio = j.at("basic_audio");
    result.ycbcr_444 = j.at("ycbcr_444");
    result.ycbcr_422 = j.at("ycbcr_422");
    auto data_block_collection = j.at("data_block_collection").get<DataBlockCollection>();
    for (const auto& data_block : data_block_collection) {
      result.data_block_collection.push_back(data_block);
    }
    auto detailed_timing_descriptors = j.at("detailed_timing_descriptors").get<std::vector<DetailedTimingDescriptor>>();
    for (const auto& detailed_timing_descriptor : detailed_timing_descriptors) {
      result.detailed_timing_descriptors.push_back(detailed_timing_descriptor);
    }
  }

  void to_json(nlohmann::json& j, const Cta861Block& block) {
    j["underscan"] = block.underscan;
    j["basic_audio"] = block.basic_audio;
    j["ycbcr_444"] = block.ycbcr_444;
    j["ycbcr_422"] = block.ycbcr_422;

    j["data_block_collection"] = nlohmann::json::array();
    j["detailed_timing_descriptors"] = nlohmann::json::array();
    for (const CtaDataBlock& data_block : block.data_block_collection) {
      auto d_json = std::visit([](const auto& d) -> nlohmann::json {
        nlohmann::json result;
        to_json(result, d);
        return result;
      }, data_block);
      j["data_block_collection"].push_back(d_json);
    }
    for (const auto& timing : block.detailed_timing_descriptors) {
      j["detailed_timing_descriptors"].push_back(timing);
    }
  }

  void from_json(const nlohmann::json& j, EdidData& result) {
    result.base_block = j.at("base_block");
    if (j.contains("extension_blocks")) {
      result.extension_blocks = j.at("extension_blocks").get<std::vector<Cta861Block>>();
    }
  }

  void to_json(nlohmann::json& j, const EdidData& block) {
    j["base_block"] = block.base_block;
    if (block.extension_blocks.has_value()) {
      j["extension_blocks"] = block.extension_blocks.value();
    }
  }

  void from_json(const nlohmann::json& j, Vic3dSupport& result) {
    result.vic_index = j.at("vic_index");
    result.format = j.at("format");
    if (j.contains("subsampling_3d")) {
      result.subsampling_3d = j.at("subsampling_3d").get<StereoVideoSubsampling>();
    }
  }

  void to_json(nlohmann::json& j, const Vic3dSupport& block) {
    j["vic_index"] = block.vic_index;
    j["format"] = block.format;
    if (block.subsampling_3d.has_value()) {
      j["subsampling_3d"] = block.subsampling_3d.value();
    }
  }

  void from_json(const nlohmann::json& j, StereoVideoSupport& result) {
    if (j.contains("formats")) {
      std::pair<uint8_t, uint8_t> fmts;

      const auto& formats = j.at("formats").get<std::vector<nlohmann::json>>();
      for (const auto& format : formats) {
        StereoVideoFormatByte1 byte_1 = format;
        StereoVideoFormatByte2 byte_2 = format;

        if (byte_1 != ENUM_NULL) {
          fmts.first |= byte_1;
        }
        else if (byte_2 != ENUM_NULL) {
          fmts.second |= byte_2;
        }
      }

      result.formats = std::move(fmts);
    }
    if (j.contains("vics")) {
      uint16_t vics = 0;
      for (uint8_t vic : j.at("vics").get<std::vector<uint8_t>>()) {
        vics |= 1 << (vic - 1);
      }
      result.vics = vics;
    }
  }

  void to_json(nlohmann::json& j, const StereoVideoSupport& block) {
    if (block.formats.has_value()) {
      nlohmann::json result;

      const auto push_to_json_array = [&result] (const auto& fmt) {
        if (fmt != ENUM_NULL) {
          result.push_back(fmt);
        }
      };

      for (StereoVideoFormatByte1 et : bitfield_to_enums<StereoVideoFormatByte1>(block.formats->first))
        push_to_json_array(et);
      for (StereoVideoFormatByte2 et : bitfield_to_enums<StereoVideoFormatByte2>(block.formats->second))
        push_to_json_array(et);

      j["formats"] = std::move(result);
    }
    if (block.vics.has_value()) {
      const auto vics = block.vics.value();
      for (int i = 0; i < 16; ++i) {
        if (vics >> i & BITMASK_TRUE(1)) {
          j["vics"].push_back(i + 1);
        }
      }
    }
  }

  void from_json(const nlohmann::json& j, HdmiVideoSubblock& result) {
    result.image_size_meaning = j.at("image_size_meaning");
    for (uint8_t vic : j.at("hdmi_vics").get<std::vector<uint8_t>>()) {
      result.hdmi_vics.push_back(vic);
    }
    if (j.contains("stereo_video_support")) {
      result.stereo_video_support = j.at("stereo_video_support");
    }
    for (const auto& vic_3d_item : j.at("vic_3d_support").get<std::vector<Vic3dSupport>>()) {
      result.vic_3d_support.push_back(vic_3d_item);
    }
  }

  void to_json(nlohmann::json& j, const HdmiVideoSubblock& block) {
    j["image_size_meaning"] = block.image_size_meaning;
    j["hdmi_vics"] = block.hdmi_vics;
    if (block.stereo_video_support.has_value()) {
      j["stereo_video_support"] = block.stereo_video_support.value();
    }
    j["vic_3d_support"] = block.vic_3d_support;
  }

  void from_json(const nlohmann::json& j, HdmiVendorDataBlock& result) {
    const auto source_phy_addr = j.at("source_phy_addr").get<std::vector<uint8_t>>();
    for (int i = 0; i < 4; ++i) {
      result.source_phy_addr[i] = source_phy_addr[i];
    }
    if (j.contains("capabilities")) {
      const auto capabilities = j.at("capabilities").get<std::vector<HdmiVendorDataBlockCapabilities>>();
      HdmiVendorDataBlockCapabilities caps = 0;
      for (auto cap : capabilities) {
        caps |= cap;
      }
      result.capabilities = caps;
    }
    if (j.contains("max_tmds_clock_mhz")) {
      result.max_tmds_clock_mhz = j.at("max_tmds_clock_mhz");
    }
    const auto content_types = j.at("content_types").get<std::vector<ContentTypes>>();
    for (auto type : content_types) {
      result.content_types |= type;
    }
    if (j.contains("latency")) {
      result.latency = { j.at("latency").at("video"), j.at("latency").at("audio") };
    }
    if (j.contains("interlaced_latency")) {
      result.latency = { j.at("interlaced_latency").at("video"), j.at("interlaced_latency").at("audio") };
    }
    if (j.contains("hdmi_video")) {
      result.hdmi_video = j.at("hdmi_video");
    }
  }

  void to_json(nlohmann::json& j_, const HdmiVendorDataBlock& block) {
    nlohmann::json j;

    j["source_phy_addr"] = block.source_phy_addr;
    if (block.capabilities.has_value()) {
      for (HdmiVendorDataBlockByte6Flags cap : bitfield_to_enums<HdmiVendorDataBlockByte6Flags>(block.capabilities.value()))
        j["capabilities"].push_back(to_string(cap));
    }
    if (block.max_tmds_clock_mhz.has_value()) {
      j["max_tmds_clock_mhz"] = block.max_tmds_clock_mhz.value();
    }
    for (ContentType type : bitfield_to_enums<ContentType>(block.content_types))
      j["content_types"].push_back(to_string(type));
    if (block.latency.has_value()) {
      j["latency"]["video"] = block.latency.value().first;
      j["latency"]["audio"] = block.latency.value().second;
    }
    if (block.interlaced_latency.has_value()) {
      j["interlaced_latency"]["video"] = block.interlaced_latency.value().first;
      j["interlaced_latency"]["audio"] = block.interlaced_latency.value().second;
    }
    if (block.hdmi_video.has_value()) {
      j["hdmi_video"] = block.hdmi_video.value();
    }

    j_["hdmi_vsdb"] = j;
  }
}  // namespace Edid
