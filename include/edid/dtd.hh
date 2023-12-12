// Copyright 2023 N-Nagorny
#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <variant>

#ifdef ENABLE_JSON
#include <nlohmann/json.hpp>
#endif

#include "common.hh"
#include "eighteen_byte_descriptor_interface.hh"

namespace Edid {

  enum StereoMode {
    NO_STEREO = 0b000,
    FIELD_SEQUENTIAL_L_R = 0b010,
    FIELD_SEQUENTIAL_R_L = 0b100,
    INTERLEAVED_RIGHT_EVEN = 0b011,
    INTERLEAVED_LEFT_EVEN = 0b101,
    FOUR_WAY_INTERLEAVED = 0b110,
    SIDE_BY_SIDE_INTERLEAVED = 0b111
  };

  struct AnalogCompositeSync {
    bool bipolar = false;
    bool serrations = false;
    bool sync_on_rgb_signals = false;

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AnalogCompositeSync, bipolar, serrations, sync_on_rgb_signals)
#endif
  };

#define FIELDS(X) X.bipolar, X.serrations, X.sync_on_rgb_signals
  TIED_COMPARISONS(AnalogCompositeSync, FIELDS)
#undef FIELDS

  struct DigitalCompositeSync {
    bool serrations = false;
    bool h_sync_polarity = false;

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DigitalCompositeSync, serrations, h_sync_polarity)
#endif
  };

#define FIELDS(X) X.serrations, X.h_sync_polarity
  TIED_COMPARISONS(DigitalCompositeSync, FIELDS)
#undef FIELDS

  struct DigitalSeparateSync {
    bool v_sync_polarity = false;
    bool h_sync_polarity = false;

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DigitalSeparateSync, v_sync_polarity, h_sync_polarity)
#endif
  };

#define FIELDS(X) X.v_sync_polarity, X.h_sync_polarity
  TIED_COMPARISONS(DigitalSeparateSync, FIELDS)
#undef FIELDS

  struct DtdFeaturesBitmap {
    bool interlaced = false;
    StereoMode stereo_mode = StereoMode::NO_STEREO;
    std::variant<AnalogCompositeSync, DigitalCompositeSync, DigitalSeparateSync> sync;
  };

#define FIELDS(X) X.interlaced, X.stereo_mode, X.sync
  TIED_COMPARISONS(DtdFeaturesBitmap, FIELDS)
#undef FIELDS

  struct DetailedTimingDescriptor : IEighteenByteDescriptor {
    uint64_t pixel_clock_hz = 0;  // This value becomes uint16_t in EDID by dividing by 10'000
    uint16_t h_res = 0;
    uint16_t v_res = 0;
    uint16_t h_blanking = 0;
    uint16_t v_blanking = 0;
    uint16_t h_front_porch = 0;
    uint16_t h_sync_width = 0;
    uint8_t v_front_porch = 0;
    uint8_t v_sync_width = 0;
    uint16_t h_image_size = 0;
    uint16_t v_image_size = 0;
    uint8_t h_border_pixels = 0;
    uint8_t v_border_lines = 0;
    DtdFeaturesBitmap features_bitmap;

    DetailedTimingDescriptor() = default;

    // for brace-enclosed initialization despite
    // inheritance from a base class with virtual funcs
    DetailedTimingDescriptor(
      uint64_t pixel_clock_hz_,
      uint16_t h_res_,
      uint16_t v_res_,
      uint16_t h_blanking_,
      uint16_t v_blanking_,
      uint16_t h_front_porch_,
      uint16_t h_sync_width_,
      uint8_t v_front_porch_,
      uint8_t v_sync_width_,
      uint16_t h_image_size_,
      uint16_t v_image_size_,
      uint8_t h_border_pixels_,
      uint8_t v_border_lines_,
      DtdFeaturesBitmap features_bitmap_
    )
      : pixel_clock_hz(pixel_clock_hz_)
      , h_res(h_res_)
      , v_res(v_res_)
      , h_blanking(h_blanking_)
      , v_blanking(v_blanking_)
      , h_front_porch(h_front_porch_)
      , h_sync_width(h_sync_width_)
      , v_front_porch(v_front_porch_)
      , v_sync_width(v_sync_width_)
      , h_image_size(h_image_size_)
      , v_image_size(v_image_size_)
      , h_border_pixels(h_border_pixels_)
      , v_border_lines(v_border_lines_)
      , features_bitmap(features_bitmap_)
    {}

    std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;
    static DetailedTimingDescriptor parse_byte_block(const uint8_t* start);

    uint8_t type() const override;
  };

#define FIELDS(X) X.pixel_clock_hz, X.h_res, X.v_res, X.h_blanking, X.v_blanking,      \
                  X.h_front_porch, X.h_sync_width, X.v_front_porch, X.v_sync_width,    \
                  X.h_image_size, X.v_image_size, X.h_border_pixels, X.v_border_lines, \
                  X.features_bitmap
  TIED_COMPARISONS(DetailedTimingDescriptor, FIELDS)
#undef FIELDS

}  // namespace Edid
