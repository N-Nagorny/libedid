#include "edid/base_block.hh"
#include "edid/json.hh"

namespace Edid {
  void from_json(const nlohmann::json& j, StandardTiming& timing) {
    StandardTiming result;

    result.x_resolution = j.at("x_resolution");
    result.aspect_ratio = AspectRatio(j.at("aspect_ratio"));
    result.v_frequency = j.at("v_frequency");

    timing = std::move(result);
  }

  void to_json(nlohmann::json& j, const StandardTiming& timing) {
    nlohmann::json result;
    result["x_resolution"] = timing.x_resolution;
    result["aspect_ratio"] = to_string(timing.aspect_ratio);
    result["v_frequency"] = timing.v_frequency;
    j = std::move(result);
  }

  void from_json(const nlohmann::json& j, AnalogCompositeSync& sync) {
    AnalogCompositeSync result;

    result.bipolar = j.at("bipolar");
    result.serrations = j.at("serrations");
    result.sync_on_rgb_signals = j.at("sync_on_rgb_signals");

    sync = std::move(result);
  }

  void to_json(nlohmann::json& j, const AnalogCompositeSync& timing) {
    nlohmann::json result;
    result["bipolar"] = timing.bipolar;
    result["serrations"] = timing.serrations;
    result["sync_on_rgb_signals"] = timing.sync_on_rgb_signals;
    j = std::move(result);
  }

  void from_json(const nlohmann::json& j, DigitalCompositeSync& sync) {
    DigitalCompositeSync result;

    result.serrations = j.at("serrations");
    result.h_sync_polarity = j.at("h_sync_polarity");

    sync = std::move(result);
  }

  void to_json(nlohmann::json& j, const DigitalCompositeSync& timing) {
    nlohmann::json result;
    result["serrations"] = timing.serrations;
    result["h_sync_polarity"] = timing.h_sync_polarity;
    j = std::move(result);
  }

  void from_json(const nlohmann::json& j, DigitalSeparateSync& sync) {
    DigitalSeparateSync result;

    result.v_sync_polarity = j.at("v_sync_polarity");
    result.h_sync_polarity = j.at("h_sync_polarity");

    sync = std::move(result);
  }

  void to_json(nlohmann::json& j, const DigitalSeparateSync& timing) {
    nlohmann::json result;
    result["v_sync_polarity"] = timing.v_sync_polarity;
    result["h_sync_polarity"] = timing.h_sync_polarity;
    j = std::move(result);
  }

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

  void from_json(const nlohmann::json& j, DisplayName& display_name) {
    DisplayName result;

    result.name = j.at("display_product_name");

    display_name = std::move(result);
  }

  void to_json(nlohmann::json& j, const DisplayName& display_name) {
    nlohmann::json result;
    result["display_product_name"] = display_name.name;
    j = std::move(result);
  }

  void from_json(const nlohmann::json& j, DisplayRangeLimits& limits) {
    DisplayRangeLimits result;

    result.min_v_rate_hz = j.at("min_v_rate_hz");
    result.max_v_rate_hz = j.at("max_v_rate_hz");
    result.min_h_rate_khz = j.at("min_h_rate_khz");
    result.max_h_rate_khz = j.at("max_h_rate_khz");
    result.max_pixel_clock_rate_mhz = j.at("max_pixel_clock_rate_mhz");
    result.vts = VideoTimingSupport(j.at("video_timing_support"));

    limits = std::move(result);
  }

  void to_json(nlohmann::json& j, const DisplayRangeLimits& limits) {
    nlohmann::json result;

    result["min_v_rate_hz"] = limits.min_v_rate_hz;
    result["max_v_rate_hz"] = limits.max_v_rate_hz;
    result["min_h_rate_khz"] = limits.min_h_rate_khz;
    result["max_h_rate_khz"] = limits.max_h_rate_khz;
    result["max_pixel_clock_rate_mhz"] = limits.max_pixel_clock_rate_mhz;
    result["video_timing_support"] = to_string(limits.vts);

    j = std::move(result);
  }

  void from_json(const nlohmann::json& j, DisplaySerialNumber& display_serial_number) {
    DisplaySerialNumber result;

    result.display_serial_number = j.at("serial_number");

    display_serial_number = std::move(result);
  }

  void to_json(nlohmann::json& j, const DisplaySerialNumber& display_serial_number) {
    nlohmann::json result;

    result["serial_number"] = display_serial_number.display_serial_number;

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
    for (EstablishedTiming1 et : bitfield_to_enums<EstablishedTiming1>(base_block.established_timings_1))
      result["established_timings_1"].push_back(to_string(et));
    for (EstablishedTiming2 et : bitfield_to_enums<EstablishedTiming2>(base_block.established_timings_2))
      result["established_timings_2"].push_back(to_string(et));
    for (EstablishedTiming3 et : bitfield_to_enums<EstablishedTiming3>(base_block.established_timings_3))
      result["established_timings_3"].push_back(to_string(et));
    for (const auto& timing : base_block.standard_timings) {
      if (timing.has_value()) {
        result["standard_timings"].push_back(timing.value());
      }
    }
    auto TypeOfIntegral = Overload {                                      // (2)
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
        auto d_json = std::visit(TypeOfIntegral, descriptor.value());
        if (d_json.has_value())
          result["eighteen_byte_descriptors"].push_back(d_json.value());
      }
    }
    j = std::move(result);
  }
}
