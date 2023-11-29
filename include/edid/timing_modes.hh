// Copyright 2023 N-Nagorny
#pragma once

#include <map>
#include <utility>
#include <vector>

#include "base_block.hh"
#include "cta861_block.hh"
#include "edid.hh"

namespace Edid {
  static const std::vector<uint8_t> hdmi_vic_to_vic_map = { 95, 94, 93, 98 };

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

  const std::map<EstablishedTiming1, VideoTimingMode> et_1_to_video_mode = {
    {ET_720x400_70, { 720, 400, {70, 1}, false }},
    {ET_720x400_88, { 720, 400, {88, 1}, false }},
    {ET_640x480_60, { 640, 480, {60, 1}, false }},
    {ET_640x480_67, { 640, 480, {67, 1}, false }},
    {ET_640x480_72, { 640, 480, {72, 1}, false }},
    {ET_640x480_75, { 640, 480, {75, 1}, false }},
    {ET_800x600_56, { 800, 600, {56, 1}, false }},
    {ET_800x600_60, { 800, 600, {60, 1}, false }}
  };

  const std::map<EstablishedTiming2, VideoTimingMode> et_2_to_video_mode = {
    {ET_800x600_72,   { 800, 600,   {72, 1}, false }},
    {ET_800x600_75,   { 800, 600,   {75, 1}, false }},
    {ET_832x624_75,   { 832, 624,   {75, 1}, false }},
    {ET_1024x768i_87, { 1024, 768,  {87, 1}, true }},
    {ET_1024x768_60,  { 1024, 768,  {60, 1}, false }},
    {ET_1024x768_70,  { 1024, 768,  {70, 1}, false }},
    {ET_1024x768_75,  { 1024, 768,  {75, 1}, false }},
    {ET_1280x1024_75, { 1280, 1024, {75, 1}, false }}
  };

  const std::map<ManufacturersTiming, VideoTimingMode> mt_to_video_mode = {
    {ET_1152x870_75,   { 1152, 870, {75, 1}, false }}
  };

  const std::map<std::pair<uint8_t, uint8_t>, VideoTimingMode> et_3_to_video_mode = {
    {{0, ET_640x350_85},   { 640, 350,   {85, 1}, false }},
    {{0, ET_640x400_85},   { 640, 400,   {85, 1}, false }},
    {{0, ET_720x400_85},   { 720, 400,   {85, 1}, false }},
    {{0, ET_640x480_85},   { 640, 480,   {85, 1}, false }},
    {{0, ET_848x480_60},   { 848, 480,   {60, 1}, false }},
    {{0, ET_800x600_85},   { 800, 600,   {85, 1}, false }},
    {{0, ET_1024x768_85},  { 1024, 768,  {85, 1}, false }},
    {{0, ET_1152x864_75},  { 1152, 864,  {75, 1}, false }},

    {{1, ET_1280x768_60_RB},   { 1280, 768,   {60, 1}, false }},
    {{1, ET_1280x768_60},      { 1280, 768,   {60, 1}, false }},
    {{1, ET_1280x768_75},      { 1280, 768,   {75, 1}, false }},
    {{1, ET_1280x768_85},      { 1280, 768,   {85, 1}, false }},
    {{1, ET_1280x960_60},      { 1280, 960,   {60, 1}, false }},
    {{1, ET_1280x960_85},      { 1280, 960,   {85, 1}, false }},
    {{1, ET_1280x1024_60},     { 1280, 1024,  {60, 1}, false }},
    {{1, ET_1280x1024_85},     { 1280, 1024,  {85, 1}, false }},

    {{2, ET_1360x768_60},      { 1360, 768,   {60, 1}, false }},
    {{2, ET_1440x900_60_RB},   { 1440, 900,   {60, 1}, false }},
    {{2, ET_1440x900_60},      { 1440, 900,   {60, 1}, false }},
    {{2, ET_1440x900_75},      { 1440, 900,   {75, 1}, false }},
    {{2, ET_1440x900_85},      { 1440, 900,   {85, 1}, false }},
    {{2, ET_1440x1050_60_RB},  { 1440, 1050,  {60, 1}, false }},
    {{2, ET_1440x1050_60},     { 1440, 1050,  {60, 1}, false }},
    {{2, ET_1440x1050_75},     { 1440, 1050,  {75, 1}, false }},

    {{3, ET_1400x1050_85},     { 1440, 1050,  {85, 1}, false }},
    {{3, ET_1680x1050_60_RB},  { 1680, 1050,  {60, 1}, false }},
    {{3, ET_1680x1050_60},     { 1680, 1050,  {60, 1}, false }},
    {{3, ET_1680x1050_75},     { 1680, 1050,  {75, 1}, false }},
    {{3, ET_1680x1050_85},     { 1680, 1050,  {85, 1}, false }},
    {{3, ET_1600x1200_60},     { 1600, 1200,  {60, 1}, false }},
    {{3, ET_1600x1200_65},     { 1600, 1200,  {65, 1}, false }},
    {{3, ET_1600x1200_70},     { 1600, 1200,  {70, 1}, false }},

    {{4, ET_1600x1200_75},     { 1600, 1200,  {75, 1}, false }},
    {{4, ET_1600x1200_85},     { 1600, 1200,  {85, 1}, false }},
    {{4, ET_1792x1344_60},     { 1792, 1344,  {60, 1}, false }},
    {{4, ET_1792x1344_75},     { 1792, 1344,  {75, 1}, false }},
    {{4, ET_1856x1392_60},     { 1856, 1392,  {60, 1}, false }},
    {{4, ET_1856x1392_75},     { 1856, 1392,  {75, 1}, false }},
    {{4, ET_1920x1200_60_RB},  { 1920, 1200,  {60, 1}, false }},
    {{4, ET_1920x1200_60},     { 1920, 1200,  {60, 1}, false }},

    {{5, ET_1920x1200_75},     { 1920, 1200,  {75, 1}, false }},
    {{5, ET_1920x1200_85},     { 1920, 1200,  {85, 1}, false }},
    {{5, ET_1920x1440_60},     { 1920, 1440,  {60, 1}, false }},
    {{5, ET_1920x1440_75},     { 1920, 1440,  {75, 1}, false }}
  };

  const std::map<std::pair<uint8_t, uint8_t>, VideoTimingMode> std_2_byte_code_to_video_mode = {
    { {0x31, 0x19}, { 640, 400,   { 85, 1 }, false } },
    { {0x31, 0x40}, { 640, 480,   { 60, 1 }, false } },
    { {0x31, 0x4c}, { 640, 480,   { 72, 1 }, false } },
    { {0x31, 0x4f}, { 640, 480,   { 75, 1 }, false } },
    { {0x31, 0x59}, { 640, 480,   { 85, 1 }, false } },
    { {0x45, 0x40}, { 800, 600,   { 60, 1 }, false } },
    { {0x45, 0x4c}, { 800, 600,   { 72, 1 }, false } },
    { {0x45, 0x4f}, { 800, 600,   { 75, 1 }, false } },
    { {0x45, 0x59}, { 800, 600,   { 85, 1 }, false } },
    { {0x61, 0x40}, { 1024, 768,  { 60, 1 }, false } },
    { {0x61, 0x4c}, { 1024, 768,  { 70, 1 }, false } },
    { {0x61, 0x4f}, { 1024, 768,  { 75, 1 }, false } },
    { {0x61, 0x59}, { 1024, 768,  { 85, 1 }, false } },
    { {0x71, 0x4f}, { 1152, 864,  { 75, 1 }, false } },
    { {0x81, 0xc0}, { 1280, 720,  { 60, 1 }, false } },
    { {0x81, 0x00}, { 1280, 800,  { 60, 1 }, false } },
    { {0x81, 0x0f}, { 1280, 800,  { 75, 1 }, false } },
    { {0x81, 0x19}, { 1280, 800,  { 85, 1 }, false } },
    { {0x81, 0x40}, { 1280, 960,  { 60, 1 }, false } },
    { {0x81, 0x59}, { 1280, 960,  { 85, 1 }, false } },
    { {0x81, 0x80}, { 1280, 1024, { 60, 1 }, false } },
    { {0x81, 0x8f}, { 1280, 1024, { 75, 1 }, false } },
    { {0x81, 0x99}, { 1280, 1024, { 85, 1 }, false } },
    { {0x90, 0x40}, { 1400, 1050, { 60, 1 }, false } },
    { {0x90, 0x4f}, { 1400, 1050, { 75, 1 }, false } },
    { {0x90, 0x59}, { 1400, 1050, { 85, 1 }, false } },
    { {0x95, 0x00}, { 1440, 900,  { 60, 1 }, false } },
    { {0x95, 0x0f}, { 1440, 900,  { 75, 1 }, false } },
    { {0x95, 0x19}, { 1440, 900,  { 85, 1 }, false } },
    { {0xa9, 0xc0}, { 1600, 900,  { 60, 1 }, false } },
    { {0xa9, 0x40}, { 1600, 1200, { 60, 1 }, false } },
    { {0xa9, 0x45}, { 1600, 1200, { 65, 1 }, false } },
    { {0xa9, 0x4a}, { 1600, 1200, { 70, 1 }, false } },
    { {0xa9, 0x4f}, { 1600, 1200, { 75, 1 }, false } },
    { {0xa9, 0x59}, { 1600, 1200, { 85, 1 }, false } },
    { {0xb3, 0x00}, { 1680, 1050, { 60, 1 }, false } },
    { {0xb3, 0x0f}, { 1680, 1050, { 75, 1 }, false } },
    { {0xb3, 0x19}, { 1680, 1050, { 85, 1 }, false } },
    { {0xc1, 0x40}, { 1792, 1344, { 60, 1 }, false } },
    { {0xc1, 0x4f}, { 1792, 1344, { 75, 1 }, false } },
    { {0xc9, 0x40}, { 1856, 1392, { 60, 1 }, false } },
    { {0xc9, 0x4f}, { 1856, 1392, { 75, 1 }, false } },
    { {0xd1, 0xc0}, { 1920, 1080, { 60, 1 }, false } },
    { {0xd1, 0x00}, { 1920, 1200, { 60, 1 }, false } },
    { {0xd1, 0x0f}, { 1920, 1200, { 75, 1 }, false } },
    { {0xd1, 0x19}, { 1920, 1200, { 85, 1 }, false } },
    { {0xd1, 0x40}, { 1920, 1440, { 60, 1 }, false } },
    { {0xd1, 0x4f}, { 1920, 1440, { 75, 1 }, false } },
    { {0xe1, 0xc0}, { 2048, 1152, { 60, 1 }, false } }
  };

  std::optional<Cta861VideoTimingMode> get_cta861_video_timing_mode(uint8_t vic);

  VideoTimingMode to_video_timing_mode(const DetailedTimingDescriptor& dtd, uint8_t pixel_repetition_factor = 1);

  template <class Function>
  Function for_each_mode(const BaseBlock& base_block, Function fn) {
    for (EstablishedTiming1 et : bitfield_to_enums<EstablishedTiming1>(base_block.established_timings_1)) {
      fn(et_1_to_video_mode.at(et));
    }
    for (EstablishedTiming2 et : bitfield_to_enums<EstablishedTiming2>(base_block.established_timings_2)) {
      fn(et_2_to_video_mode.at(et));
    }
    for (ManufacturersTiming et : bitfield_to_enums<ManufacturersTiming>(base_block.manufacturers_timings)) {
      fn(mt_to_video_mode.at(et));
    }

    for (const auto& standard_timing : base_block.standard_timings) {
      if (standard_timing.has_value()) {
        const auto std_2_byte_code = generate_standard_timing(standard_timing);
        if (std_2_byte_code_to_video_mode.count(std_2_byte_code) != 0) {
          fn(std_2_byte_code_to_video_mode.at(std_2_byte_code));
        }
      }
    }

    for (const auto& descriptor : base_block.eighteen_byte_descriptors) {
      if (descriptor.has_value()) {
        if (std::visit(is_dtd_visitor, descriptor.value())) {
          fn(to_video_timing_mode(std::get<DetailedTimingDescriptor>(descriptor.value())));
        }
        else if (std::visit(is_et3_visitor, descriptor.value())) {
          const auto& established_timings_3 = std::get<EstablishedTimings3>(descriptor.value());
          for (int i = 0; i < 6; ++i) {
            for (uint8_t et : bitfield_to_enums<EstablishedTiming3Byte6>(established_timings_3.bytes_6_11.at(i))) {
              fn(et_3_to_video_mode.at({i, et}));
            }
          }
        }
      }
    }

    return fn;
  }

  template <class Function>
  void remove_mode_if(BaseBlock& base_block, Function fn, bool remove_unknown = false) {
    for (EstablishedTiming1 et : bitfield_to_enums<EstablishedTiming1>(base_block.established_timings_1)) {
      if (fn(et_1_to_video_mode.at(et))) {
        base_block.established_timings_1 &= ~et;
      }
    }
    for (EstablishedTiming2 et : bitfield_to_enums<EstablishedTiming2>(base_block.established_timings_2)) {
      if (fn(et_2_to_video_mode.at(et))) {
        base_block.established_timings_2 &= ~et;
      }
    }
    for (ManufacturersTiming et : bitfield_to_enums<ManufacturersTiming>(base_block.manufacturers_timings)) {
      if (fn(mt_to_video_mode.at(et))) {
        base_block.manufacturers_timings &= ~et;
      }
    }

    for (auto& standard_timing : base_block.standard_timings) {
      if (standard_timing.has_value()) {
        const auto std_2_byte_code = generate_standard_timing(standard_timing);
        if (std_2_byte_code_to_video_mode.count(std_2_byte_code) != 0) {
          if (fn(std_2_byte_code_to_video_mode.at(std_2_byte_code))) {
            standard_timing = std::nullopt;
          }
        }
        else {
          if (remove_unknown) {
            standard_timing = std::nullopt;
          }
        }
      }
    }

    for (auto& descriptor : base_block.eighteen_byte_descriptors) {
      if (descriptor.has_value()) {
        if (std::visit(is_dtd_visitor, descriptor.value())) {
          if (fn(to_video_timing_mode(std::get<DetailedTimingDescriptor>(descriptor.value())))) {
            descriptor = std::nullopt;
          }
        }
        else if (std::visit(is_et3_visitor, descriptor.value())) {
          auto& established_timings_3 = std::get<EstablishedTimings3>(descriptor.value());
          for (int i = 0; i < 6; ++i) {
            for (uint8_t et : bitfield_to_enums<EstablishedTiming3Byte6>(established_timings_3.bytes_6_11.at(i))) {
              if (fn(et_3_to_video_mode.at({i, et}))) {
                established_timings_3.bytes_6_11.at(i) &= ~et;
              }
            }
          }
        }
      }
    }
  }

  template <class Function>
  Function for_each_mode(const Cta861Block& cta861_block, Function fn) {
    for (const auto& data_block : cta861_block.data_block_collection) {
      if (std::visit(is_vdb_visitor, data_block)) {
        const auto& vics = std::get<VideoDataBlock>(data_block).vics;
        for (const std::optional<uint8_t>& vic : vics) {
          if (vic.has_value()) {
            auto mode = get_cta861_video_timing_mode(vic.value());
            if (mode.has_value()) {
              uint8_t pixel_repetition_factor = 1;
              if (std::holds_alternative<uint8_t>(mode->pixel_repetition_factor)) {
                pixel_repetition_factor = std::get<uint8_t>(mode->pixel_repetition_factor);
              }
              else if (std::holds_alternative<std::pair<uint8_t, uint8_t>>(mode->pixel_repetition_factor)) {
                pixel_repetition_factor = std::get<std::pair<uint8_t, uint8_t>>(mode->pixel_repetition_factor).first;
              }
              else if (std::holds_alternative<std::vector<uint8_t>>(mode->pixel_repetition_factor)) {
                pixel_repetition_factor = std::get<std::vector<uint8_t>>(mode->pixel_repetition_factor).at(0);
              }

              fn(to_video_timing_mode(mode->dtd, pixel_repetition_factor));
            }
          }
        }
      }
      else if (std::visit(is_hdmi_vsdb_visitor, data_block)) {
        const auto& hdmi_vsdb = std::get<HdmiVendorDataBlock>(data_block);
        if (hdmi_vsdb.hdmi_video.has_value()) {
          for (uint8_t hdmi_vic : hdmi_vsdb.hdmi_video->hdmi_vics) {
            uint8_t vic = hdmi_vic_to_vic_map.at(hdmi_vic - 1);
            auto mode = get_cta861_video_timing_mode(vic);
            if (mode.has_value()) {
              uint8_t pixel_repetition_factor = 1;
              if (std::holds_alternative<uint8_t>(mode->pixel_repetition_factor)) {
                pixel_repetition_factor = std::get<uint8_t>(mode->pixel_repetition_factor);
              }
              else if (std::holds_alternative<std::pair<uint8_t, uint8_t>>(mode->pixel_repetition_factor)) {
                pixel_repetition_factor = std::get<std::pair<uint8_t, uint8_t>>(mode->pixel_repetition_factor).first;
              }
              else if (std::holds_alternative<std::vector<uint8_t>>(mode->pixel_repetition_factor)) {
                pixel_repetition_factor = std::get<std::vector<uint8_t>>(mode->pixel_repetition_factor).at(0);
              }

              fn(to_video_timing_mode(mode->dtd, pixel_repetition_factor));
            }
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
  void remove_mode_if(Cta861Block& cta861_block, Function fn, bool remove_unknown = false) {
    for (auto& data_block : cta861_block.data_block_collection) {
      if (std::visit(is_vdb_visitor, data_block)) {
        auto& vics = std::get<VideoDataBlock>(data_block).vics;
        for (std::optional<uint8_t>& vic : vics) {
          if (vic.has_value()) {
            auto mode = get_cta861_video_timing_mode(vic.value());
            if (mode.has_value()) {
              uint8_t pixel_repetition_factor = 1;
              if (std::holds_alternative<uint8_t>(mode->pixel_repetition_factor)) {
                pixel_repetition_factor = std::get<uint8_t>(mode->pixel_repetition_factor);
              }
              else if (std::holds_alternative<std::pair<uint8_t, uint8_t>>(mode->pixel_repetition_factor)) {
                pixel_repetition_factor = std::get<std::pair<uint8_t, uint8_t>>(mode->pixel_repetition_factor).first;
              }
              else if (std::holds_alternative<std::vector<uint8_t>>(mode->pixel_repetition_factor)) {
                pixel_repetition_factor = std::get<std::vector<uint8_t>>(mode->pixel_repetition_factor).at(0);
              }

              if (fn(to_video_timing_mode(mode->dtd, pixel_repetition_factor))) {
                vic = std::nullopt;
              }
            }
            else if (remove_unknown) {
              vic = std::nullopt;
            }
          }
        }
      }
      else if (std::visit(is_hdmi_vsdb_visitor, data_block)) {
        auto& hdmi_vsdb = std::get<HdmiVendorDataBlock>(data_block);

        if (hdmi_vsdb.hdmi_video.has_value()) {
          hdmi_vsdb.hdmi_video->hdmi_vics.erase(std::remove_if(
            hdmi_vsdb.hdmi_video->hdmi_vics.begin(),
            hdmi_vsdb.hdmi_video->hdmi_vics.end(),
            [remove_unknown, &fn](uint8_t hdmi_vic) {
              uint8_t vic = hdmi_vic_to_vic_map.at(hdmi_vic - 1);
              auto mode = get_cta861_video_timing_mode(vic);
              if (mode.has_value()) {
                uint8_t pixel_repetition_factor = 1;
                if (std::holds_alternative<uint8_t>(mode->pixel_repetition_factor)) {
                  pixel_repetition_factor = std::get<uint8_t>(mode->pixel_repetition_factor);
                }
                else if (std::holds_alternative<std::pair<uint8_t, uint8_t>>(mode->pixel_repetition_factor)) {
                  pixel_repetition_factor = std::get<std::pair<uint8_t, uint8_t>>(mode->pixel_repetition_factor).first;
                }
                else if (std::holds_alternative<std::vector<uint8_t>>(mode->pixel_repetition_factor)) {
                  pixel_repetition_factor = std::get<std::vector<uint8_t>>(mode->pixel_repetition_factor).at(0);
                }

                if (fn(to_video_timing_mode(mode->dtd, pixel_repetition_factor))) {
                  return true;
                }
              }
              else if (remove_unknown) {
                return true;
              }
              return false;
            }), hdmi_vsdb.hdmi_video->hdmi_vics.end()
          );
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
  void remove_mode_if(EdidData& edid, Function fn, bool remove_unknown = false) {
    remove_mode_if(edid.base_block, fn, remove_unknown);

    if (edid.extension_blocks.has_value()) {
      for (auto& ext_block : edid.extension_blocks.value()) {
        remove_mode_if(ext_block, fn, remove_unknown);
      }
    }
  }
}  // namespace Edid
