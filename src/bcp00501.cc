#include <unordered_set>

#include "edid/bcp00501.hh"
#include "edid/timing_modes.hh"

using json = nlohmann::json;

namespace Edid {
  using Ratio = std::pair<uint64_t, uint64_t>;

  json generate_constraint_set(uint16_t frame_width, uint16_t frame_height, Ratio frame_rate, bool interlaced) {
    json result;

    result["urn:x-nmos:cap:format:frame_width"]["enum"] = { frame_width };
    result["urn:x-nmos:cap:format:frame_height"]["enum"] = { frame_height };
    if (frame_rate.first % frame_rate.second == 0) {
      result["urn:x-nmos:cap:format:grain_rate"]["enum"]["numerator"] = frame_rate.first / frame_rate.second;
    }
    else {
      uint64_t numerator = frame_rate.first;
      uint64_t denominator = frame_rate.second;

      if (frame_rate.first % NTSC_FACTOR_NUMERATOR == 0 && frame_rate.second % NTSC_FACTOR_DENOMINATOR == 0) {
        numerator /= NTSC_FACTOR_NUMERATOR;
        denominator /= NTSC_FACTOR_DENOMINATOR;
        numerator /= denominator;
        numerator *= NTSC_FACTOR_NUMERATOR;
        denominator = NTSC_FACTOR_DENOMINATOR;
      }

      result["urn:x-nmos:cap:format:grain_rate"]["enum"] = {
        { "numerator", numerator },
        { "denominator", denominator },
      };
    }
    result["urn:x-nmos:cap:format:interlace_mode"]["enum"] = { interlaced };

    return result;
  }

  std::unordered_set<json> generate_constraint_sets(const BaseBlock& base_block) {
    std::unordered_set<json> result;

    for (EstablishedTiming1 et : bitfield_to_enums<EstablishedTiming1>(base_block.established_timings_1)) {
      auto mode = established_timings_1.at(et);
      result.insert(generate_constraint_set(mode.h_res, mode.v_res, std::make_pair(mode.v_rate_hz, 1), mode.interlaced));
    }
    for (EstablishedTiming2 et : bitfield_to_enums<EstablishedTiming2>(base_block.established_timings_2)) {
      auto mode = established_timings_2.at(et);
      result.insert(generate_constraint_set(mode.h_res, mode.v_res, std::make_pair(mode.v_rate_hz, 1), mode.interlaced));
    }
    for (EstablishedTiming3 et : bitfield_to_enums<EstablishedTiming3>(base_block.established_timings_3)) {
      auto mode = established_timings_3.at(et);
      result.insert(generate_constraint_set(mode.h_res, mode.v_res, std::make_pair(mode.v_rate_hz, 1), mode.interlaced));
    }

    for (const auto& mode : base_block.detailed_timing_descriptors) {
      if (mode.has_value()) {
        result.insert(generate_constraint_set(
          mode->h_res,
          mode->v_res * (mode->features_bitmap.interlaced ? 2 : 1),
          std::make_pair(mode->pixel_clock_hz, (mode->v_res + mode->v_blank_lines) * (mode->h_res + mode->h_blank_pixels)),
          mode->features_bitmap.interlaced
        ));
      }
    }

    return result;
  }

  std::unordered_set<json> generate_constraint_sets(const Cta861Block& cta861_block) {
    std::unordered_set<json> result;

    for (const std::optional<uint8_t>& vic : cta861_block.data_block_collection.video_data_block.vics) {
      if (vic.has_value()) {
        auto mode = get_cta861_video_timing_mode(vic.value());
        uint8_t pixel_repetition_factor = 1;
        if (std::holds_alternative<uint8_t>(mode.pixel_repetition_factor)) {
          pixel_repetition_factor = std::get<uint8_t>(mode.pixel_repetition_factor);
        }
        else if (std::holds_alternative<std::pair<uint8_t, uint8_t>>(mode.pixel_repetition_factor)) {
          pixel_repetition_factor = std::get<std::pair<uint8_t, uint8_t>>(mode.pixel_repetition_factor).first;
        }
        else if (std::holds_alternative<std::vector<uint8_t>>(mode.pixel_repetition_factor)) {
          pixel_repetition_factor = std::get<std::vector<uint8_t>>(mode.pixel_repetition_factor).at(0);
        }

        result.insert(generate_constraint_set(
          mode.dtd.h_res / pixel_repetition_factor,
          mode.dtd.v_res * (mode.dtd.features_bitmap.interlaced ? 2 : 1),
          std::make_pair(mode.dtd.pixel_clock_hz, (mode.dtd.v_res + mode.dtd.v_blank_lines) * (mode.dtd.h_res + mode.dtd.h_blank_pixels)),
          mode.dtd.features_bitmap.interlaced
        ));
      }
    }

    for (const auto& mode : cta861_block.detailed_timing_descriptors) {
      result.insert(generate_constraint_set(
        mode.h_res,
        mode.v_res * (mode.features_bitmap.interlaced ? 2 : 1),
        std::make_pair(mode.pixel_clock_hz, (mode.v_res + mode.v_blank_lines) * (mode.h_res + mode.h_blank_pixels)),
        mode.features_bitmap.interlaced
      ));
    }

    return result;
  }

  json generate_constraint_sets(const EdidData& edid) {
    std::unordered_set<json> constraint_sets = generate_constraint_sets(edid.base_block);
    if (edid.extension_blocks.has_value()) {
      for (const auto& ext_block : edid.extension_blocks.value()) {
        std::unordered_set<json> temp = generate_constraint_sets(ext_block);
        std::move(temp.begin(), temp.end(), std::inserter(constraint_sets, constraint_sets.end()));
      }
    }
    return json(constraint_sets);
  }
}
