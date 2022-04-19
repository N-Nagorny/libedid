#pragma once

#include <map>

#include "base_block.hh"
#include "cta861_block.hh"

namespace Edid {
  struct VideoTimingMode {
    uint16_t h_res;
    uint16_t v_res;
    uint32_t v_rate_hz;
    bool interlaced;
  };

  struct Cta861VideoTimingMode {
    DetailedTimingDescriptor dtd;
    std::variant<uint8_t, std::pair<uint8_t, uint8_t>, std::vector<uint8_t>> pixel_repetition_factor;
  };

  const std::map<EstablishedTiming1, VideoTimingMode> established_timings_1 = {
    {ET_720x400_70, { 720, 400, 70, false }},
    {ET_720x400_88, { 720, 400, 88, false }},
    {ET_640x480_60, { 640, 480, 60, false }},
    {ET_640x480_67, { 640, 480, 67, false }},
    {ET_640x480_72, { 640, 480, 72, false }},
    {ET_640x480_75, { 640, 480, 75, false }},
    {ET_800x600_56, { 800, 600, 56, false }},
    {ET_800x600_60, { 800, 600, 60, false }}
  };

  const std::map<EstablishedTiming2, VideoTimingMode> established_timings_2 = {
    {ET_800x600_72,   { 800, 600, 72, false }},
    {ET_800x600_75,   { 800, 600, 75, false }},
    {ET_832x624_75,   { 832, 624, 75, false }},
    {ET_1024x768_87i, { 1024, 768, 87, true }},
    {ET_1024x768_60,  { 1024, 768, 60, false }},
    {ET_1024x768_70,  { 1024, 768, 70, false }},
    {ET_1024x768_75,  { 1024, 768, 75, false }},
    {ET_1280x1024_75, { 1280, 1024, 75, false }}
  };

  const std::map<EstablishedTiming3, VideoTimingMode> established_timings_3 = {
    {ET_1152x870_75,   { 1152, 870, 75, false }}
  };

  Cta861VideoTimingMode get_cta861_video_timing_mode(uint8_t vic);

  template <class Function>
  void for_each_mode(const BaseBlock& base_block, Function fn) {
    for (EstablishedTiming1 et : bitfield_to_enums<EstablishedTiming1>(base_block.established_timings_1)) {
      auto mode = established_timings_1.at(et);
      fn(mode.h_res, mode.v_res, std::make_pair(mode.v_rate_hz, 1), mode.interlaced);
    }
    for (EstablishedTiming2 et : bitfield_to_enums<EstablishedTiming2>(base_block.established_timings_2)) {
      auto mode = established_timings_2.at(et);
      fn(mode.h_res, mode.v_res, std::make_pair(mode.v_rate_hz, 1), mode.interlaced);
    }
    for (EstablishedTiming3 et : bitfield_to_enums<EstablishedTiming3>(base_block.established_timings_3)) {
      auto mode = established_timings_3.at(et);
      fn(mode.h_res, mode.v_res, std::make_pair(mode.v_rate_hz, 1), mode.interlaced);
    }

    for (const auto& mode : base_block.detailed_timing_descriptors) {
      if (mode.has_value()) {
        fn(
          mode->h_res,
          mode->v_res * (mode->features_bitmap.interlaced ? 2 : 1),
          std::make_pair(mode->pixel_clock_hz, (mode->v_res + mode->v_blank_lines) * (mode->h_res + mode->h_blank_pixels)),
          mode->features_bitmap.interlaced
        );
      }
    }
  }

  template <class Function>
  void for_each_mode(const Cta861Block& cta861_block, Function fn) {
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

        fn(
          mode.dtd.h_res / pixel_repetition_factor,
          mode.dtd.v_res * (mode.dtd.features_bitmap.interlaced ? 2 : 1),
          std::make_pair(mode.dtd.pixel_clock_hz, (mode.dtd.v_res + mode.dtd.v_blank_lines) * (mode.dtd.h_res + mode.dtd.h_blank_pixels)),
          mode.dtd.features_bitmap.interlaced
        );
      }
    }

    for (const auto& mode : cta861_block.detailed_timing_descriptors) {
      fn(
        mode.h_res,
        mode.v_res * (mode.features_bitmap.interlaced ? 2 : 1),
        std::make_pair(mode.pixel_clock_hz, (mode.v_res + mode.v_blank_lines) * (mode.h_res + mode.h_blank_pixels)),
        mode.features_bitmap.interlaced
      );
    }
  }
}
