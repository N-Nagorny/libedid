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

  void from_json(const nlohmann::json& j, DetailedTimingDescriptor& dtd) {
    DetailedTimingDescriptor result;

    result.pixel_clock_hz = j.at("pixel_clock").get<uint64_t>() * 10'000;
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
    result["pixel_clock"] = dtd.pixel_clock_hz / 10'000;
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
    if (j.contains("pixel_clock")) {
      DetailedTimingDescriptor subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
    else if (j.contains("display_product_name")) {
      DisplayName subresult;
      from_json(j, subresult);
      descriptor = subresult;
    }
    else if (j.contains("serial_number")) {
      DisplaySerialNumber subresult;
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

  void from_json(const nlohmann::json& j, UnknownDataBlock& result) {
    result.raw_data = j.at("raw_data").get<std::vector<uint8_t>>();
    result.data_block_tag = j.at("tag");
  }

  void to_json(nlohmann::json& j, const UnknownDataBlock& block) {
    j["raw_data"] = block.raw_data;
    j["tag"] = block.data_block_tag;
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

  void from_json(const nlohmann::json& j, BaseBlock& base_block) {
    BaseBlock result;

    for (int i = 0; i < 3; ++i)
      result.manufacturer_id[i] = j.at("manufacturer_id").get<std::string>().at(i);
    result.product_code = j.at("product_code");
    result.serial_number = j.at("serial_number");
    result.manufacture_week = j.at("week_of_manufacture");
    result.manufacture_year = j.at("year_of_manufacture");
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
    auto et_3 = j.at("established_timings_3").get<std::vector<EstablishedTiming3>>();
    for (auto et : et_3) {
      result.established_timings_3 |= et;
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
    result["week_of_manufacture"] = base_block.manufacture_week;
    result["year_of_manufacture"] = base_block.manufacture_year;
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
    result["established_timings_3"] = nlohmann::json::array();
    for (EstablishedTiming1 et : bitfield_to_enums<EstablishedTiming1>(base_block.established_timings_1))
      result["established_timings_1"].push_back(to_string(et));
    for (EstablishedTiming2 et : bitfield_to_enums<EstablishedTiming2>(base_block.established_timings_2))
      result["established_timings_2"].push_back(to_string(et));
    for (EstablishedTiming3 et : bitfield_to_enums<EstablishedTiming3>(base_block.established_timings_3))
      result["established_timings_3"].push_back(to_string(et));
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
}
