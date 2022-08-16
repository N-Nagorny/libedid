#pragma once

#include <map>

#include "base_block.hh"
#include "cta861_block.hh"
#include "edid.hh"

namespace Edid {
  using Ratio = std::pair<uint64_t, uint64_t>;

  struct VideoTimingMode {
    uint16_t h_res;
    uint16_t v_res;
    Ratio v_rate_hz;
    bool interlaced;
  };

  bool operator==(const VideoTimingMode& lhs, const VideoTimingMode& rhs);

  struct Cta861VideoTimingMode {
    DetailedTimingDescriptor dtd;
    std::variant<uint8_t, std::pair<uint8_t, uint8_t>, std::vector<uint8_t>> pixel_repetition_factor;
  };

  const std::map<EstablishedTiming1, VideoTimingMode> established_timings_1 = {
    {ET_720x400_70, { 720, 400, {70, 1}, false }},
    {ET_720x400_88, { 720, 400, {88, 1}, false }},
    {ET_640x480_60, { 640, 480, {60, 1}, false }},
    {ET_640x480_67, { 640, 480, {67, 1}, false }},
    {ET_640x480_72, { 640, 480, {72, 1}, false }},
    {ET_640x480_75, { 640, 480, {75, 1}, false }},
    {ET_800x600_56, { 800, 600, {56, 1}, false }},
    {ET_800x600_60, { 800, 600, {60, 1}, false }}
  };

  const std::map<EstablishedTiming2, VideoTimingMode> established_timings_2 = {
    {ET_800x600_72,   { 800, 600,   {72, 1}, false }},
    {ET_800x600_75,   { 800, 600,   {75, 1}, false }},
    {ET_832x624_75,   { 832, 624,   {75, 1}, false }},
    {ET_1024x768_87i, { 1024, 768,  {87, 1}, true }},
    {ET_1024x768_60,  { 1024, 768,  {60, 1}, false }},
    {ET_1024x768_70,  { 1024, 768,  {70, 1}, false }},
    {ET_1024x768_75,  { 1024, 768,  {75, 1}, false }},
    {ET_1280x1024_75, { 1280, 1024, {75, 1}, false }}
  };

  const std::map<EstablishedTiming3, VideoTimingMode> established_timings_3 = {
    {ET_1152x870_75,   { 1152, 870, {75, 1}, false }}
  };

  Cta861VideoTimingMode get_cta861_video_timing_mode(uint8_t vic);

  VideoTimingMode to_video_timing_mode(const DetailedTimingDescriptor& dtd, uint8_t pixel_repetition_factor = 1);

  template <class Function>
  Function for_each_mode(const BaseBlock& base_block, Function fn) {
    for (EstablishedTiming1 et : bitfield_to_enums<EstablishedTiming1>(base_block.established_timings_1)) {
      fn(established_timings_1.at(et));
    }
    for (EstablishedTiming2 et : bitfield_to_enums<EstablishedTiming2>(base_block.established_timings_2)) {
      fn(established_timings_2.at(et));
    }
    for (EstablishedTiming3 et : bitfield_to_enums<EstablishedTiming3>(base_block.established_timings_3)) {
      fn(established_timings_3.at(et));
    }

    for (const auto& mode : base_block.detailed_timing_descriptors) {
      if (mode.has_value()) {
        fn(to_video_timing_mode(mode.value()));
      }
    }

    return fn;
  }

  template <class Function>
  void remove_mode_if(BaseBlock& base_block, Function fn) {
    for (EstablishedTiming1 et : bitfield_to_enums<EstablishedTiming1>(base_block.established_timings_1)) {
      if (fn(established_timings_1.at(et))) {
        base_block.established_timings_1 &= ~et;
      }
    }
    for (EstablishedTiming2 et : bitfield_to_enums<EstablishedTiming2>(base_block.established_timings_2)) {
      if (fn(established_timings_2.at(et))) {
        base_block.established_timings_2 &= ~et;
      }
    }
    for (EstablishedTiming3 et : bitfield_to_enums<EstablishedTiming3>(base_block.established_timings_3)) {
      if (fn(established_timings_3.at(et))) {
        base_block.established_timings_3 &= ~et;
      }
    }

    for (auto& mode : base_block.detailed_timing_descriptors) {
      if (mode.has_value()) {
        if (fn(to_video_timing_mode(mode.value()))) {
          mode = std::nullopt;
        }
      }
    }
  }

  template <class Function>
  Function for_each_mode(const Cta861Block& cta861_block, Function fn) {
    for (const auto& wrapper : cta861_block.data_block_collection) {
      if (wrapper.data_block_tag == CTA861_VIDEO_DATA_BLOCK_TAG) {
        const auto& vics = std::dynamic_pointer_cast<VideoDataBlock>(wrapper.data_block_ptr)->vics;
        for (const std::optional<uint8_t>& vic : vics) {
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

            fn(to_video_timing_mode(mode.dtd, pixel_repetition_factor));
          }
        }
      }
    }

    for (const auto& mode : cta861_block.detailed_timing_descriptors) {
      fn(to_video_timing_mode(mode));
    }

    return fn;
  }

  template <class Function>
  void remove_mode_if(Cta861Block& cta861_block, Function fn) {
    for (const auto& wrapper : cta861_block.data_block_collection) {
      if (wrapper.data_block_tag == CTA861_VIDEO_DATA_BLOCK_TAG) {
        auto& vics = std::dynamic_pointer_cast<VideoDataBlock>(wrapper.data_block_ptr)->vics;
        for (std::optional<uint8_t>& vic : vics) {
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

            if (fn(to_video_timing_mode(mode.dtd, pixel_repetition_factor))) {
              vic = std::nullopt;
            }
          }
        }
      }
    }

    for ( auto it = cta861_block.detailed_timing_descriptors.begin();
          it != cta861_block.detailed_timing_descriptors.end();
    ) {
      if (fn(to_video_timing_mode(*it))) {
        it = cta861_block.detailed_timing_descriptors.erase(it);
      }
      else {
        ++it;
      }
    }
  }

  template <class Function>
  Function for_each_mode(const EdidData& edid, Function fn) {
    for_each_mode(edid.base_block, fn);

    if (edid.extension_blocks.has_value()) {
      for (const auto& ext_block : edid.extension_blocks.value()) {
        for_each_mode(ext_block, fn);
      }
    }
    return fn;
  }

  template <class Function>
  void remove_mode_if(EdidData& edid, Function fn) {
    remove_mode_if(edid.base_block, fn);

    if (edid.extension_blocks.has_value()) {
      for (auto& ext_block : edid.extension_blocks.value()) {
        remove_mode_if(ext_block, fn);
      }
    }
  }
}
