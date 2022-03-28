#pragma once

#include <map>

#include "base_block.hh"

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
}
