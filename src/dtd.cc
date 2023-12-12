// Copyright 2023 N-Nagorny
#include <array>
#include <cstdint>
#include <iostream>

#include "edid/common.hh"
#include "edid/dtd.hh"
#include "edid/eighteen_byte_descriptors.hh"

namespace Edid {
  uint8_t make_dtd_features_bitmap(const DtdFeaturesBitmap& features_bitmap) {
    uint8_t result = 0;

    result |= features_bitmap.interlaced << 7;
    result |= (features_bitmap.stereo_mode & BITMASK_TRUE(2)) << 5;

    if (std::holds_alternative<AnalogCompositeSync>(features_bitmap.sync)) {
      result |= 0 << 4;
      result |= std::get<AnalogCompositeSync>(features_bitmap.sync).bipolar << 3;
      result |= std::get<AnalogCompositeSync>(features_bitmap.sync).serrations << 2;
      result |= std::get<AnalogCompositeSync>(features_bitmap.sync).sync_on_rgb_signals << 1;
    }
    else if (std::holds_alternative<DigitalCompositeSync>(features_bitmap.sync)) {
      result |= 0b10 << 3;
      result |= std::get<DigitalCompositeSync>(features_bitmap.sync).serrations << 2;
      result |= std::get<DigitalCompositeSync>(features_bitmap.sync).h_sync_polarity << 1;
    }
    else if (std::holds_alternative<DigitalSeparateSync>(features_bitmap.sync)) {
      result |= 0b11 << 3;
      result |= std::get<DigitalSeparateSync>(features_bitmap.sync).v_sync_polarity << 2;
      result |= std::get<DigitalSeparateSync>(features_bitmap.sync).h_sync_polarity << 1;
    }

    result |= features_bitmap.stereo_mode & BITMASK_TRUE(1);
    return result;
  }

  DtdFeaturesBitmap parse_dtd_features_bitmap(uint8_t features_bitmap) {
    DtdFeaturesBitmap result;
    uint8_t stereo_mode = 0;

    result.interlaced = (features_bitmap >> 7) & BITMASK_TRUE(1);

    stereo_mode = (features_bitmap >> 5) & BITMASK_TRUE(2);
    stereo_mode |= features_bitmap & BITMASK_TRUE(1);
    result.stereo_mode = StereoMode(stereo_mode);

    if (((features_bitmap >> 4) & BITMASK_TRUE(1)) == 0) {
      AnalogCompositeSync sync;
      sync.bipolar = (features_bitmap >> 3) & BITMASK_TRUE(1);
      sync.serrations = (features_bitmap >> 2) & BITMASK_TRUE(1);
      sync.sync_on_rgb_signals = (features_bitmap >> 1) & BITMASK_TRUE(1);

      result.sync = sync;
    }
    else if (((features_bitmap >> 3) & BITMASK_TRUE(2)) == 0b10) {
      DigitalCompositeSync sync;
      sync.serrations = (features_bitmap >> 2) & BITMASK_TRUE(1);
      sync.h_sync_polarity = (features_bitmap >> 1) & BITMASK_TRUE(1);

      result.sync = sync;
    }
    else if (((features_bitmap >> 3) & BITMASK_TRUE(2)) == 0b11) {
      DigitalSeparateSync sync;
      sync.v_sync_polarity = (features_bitmap >> 2) & BITMASK_TRUE(1);
      sync.h_sync_polarity = (features_bitmap >> 1) & BITMASK_TRUE(1);

      result.sync = sync;
    }
    return result;
  }

  std::array<uint8_t, EIGHTEEN_BYTES> DetailedTimingDescriptor::generate_byte_block() const {
    std::array<uint8_t, EIGHTEEN_BYTES> result;
    result.fill(0x0);
    int pos = 0;

    uint16_t pixel_clock = pixel_clock_hz / 10'000;
    result[pos++] = pixel_clock & BITMASK_TRUE(8);
    result[pos++] = (pixel_clock >> 8) & BITMASK_TRUE(8);

    result[pos++] = h_res & BITMASK_TRUE(8);
    result[pos++] = h_blanking & BITMASK_TRUE(8);
    result[pos] = ((h_res >> 8) & BITMASK_TRUE(4)) << 4;
    result[pos++] |= (h_blanking >> 8) & BITMASK_TRUE(4);

    result[pos++] = v_res & BITMASK_TRUE(8);
    result[pos++] = v_blanking & BITMASK_TRUE(8);
    result[pos] = ((v_res >> 8) & BITMASK_TRUE(4)) << 4;
    result[pos++] |= (v_blanking >> 8) & BITMASK_TRUE(4);

    result[pos++] = h_front_porch & BITMASK_TRUE(8);
    result[pos++] = h_sync_width & BITMASK_TRUE(8);
    result[pos] = (v_front_porch & BITMASK_TRUE(4)) << 4;
    result[pos++] |= v_sync_width & BITMASK_TRUE(4);
    result[pos] = ((h_front_porch >> 8) & BITMASK_TRUE(2)) << 6;
    result[pos] |= ((h_sync_width >> 8) & BITMASK_TRUE(2)) << 4;
    result[pos] |= ((v_front_porch >> 4) & BITMASK_TRUE(2)) << 2;
    result[pos++] |= ((v_sync_width >> 4) & BITMASK_TRUE(2)) << 0;

    result[pos++] = h_image_size & BITMASK_TRUE(8);
    result[pos++] = v_image_size & BITMASK_TRUE(8);
    result[pos] = ((h_image_size >> 8) & BITMASK_TRUE(4)) << 4;
    result[pos++] |= (v_image_size >> 8) & BITMASK_TRUE(4);

    result[pos++] = h_border_pixels;
    result[pos++] = v_border_lines;

    result[pos] = make_dtd_features_bitmap(features_bitmap);

    return result;
  };

  DetailedTimingDescriptor DetailedTimingDescriptor::parse_byte_block(const uint8_t* start) {
    DetailedTimingDescriptor result;
    int pos = 0;

    result.pixel_clock_hz = *(start + pos++);
    result.pixel_clock_hz |= (*(start + pos++) & BITMASK_TRUE(8)) << 8;
    result.pixel_clock_hz *= 10'000;

    result.h_res = *(start + pos++);
    result.h_blanking = *(start + pos++);
    result.h_res |= (*(start + pos) >> 4 & BITMASK_TRUE(4)) << 8;
    result.h_blanking |= (*(start + pos++) & BITMASK_TRUE(4)) << 8;

    result.v_res = *(start + pos++);
    result.v_blanking = *(start + pos++);
    result.v_res |= (*(start + pos) >> 4 & BITMASK_TRUE(4)) << 8;
    result.v_blanking |= (*(start + pos++) & BITMASK_TRUE(4)) << 8;

    result.h_front_porch = *(start + pos++);
    result.h_sync_width = *(start + pos++);
    result.v_front_porch = *(start + pos) >> 4 & BITMASK_TRUE(4);
    result.v_sync_width = *(start + pos++) & BITMASK_TRUE(4);
    result.h_front_porch |= (*(start + pos) >> 6 & BITMASK_TRUE(2)) << 8;
    result.h_sync_width |= (*(start + pos) >> 4 & BITMASK_TRUE(2)) << 8;
    result.v_front_porch |= (*(start + pos) >> 2 & BITMASK_TRUE(2)) << 4;
    result.v_sync_width |= (*(start + pos++) & BITMASK_TRUE(2)) << 4;

    result.h_image_size = *(start + pos++);
    result.v_image_size = *(start + pos++);
    result.h_image_size |= (*(start + pos) >> 4 & BITMASK_TRUE(4)) << 8;
    result.v_image_size |= (*(start + pos++) & BITMASK_TRUE(4)) << 8;

    result.h_border_pixels = *(start + pos++);
    result.v_border_lines = *(start + pos++);

    result.features_bitmap = parse_dtd_features_bitmap(*(start + pos));
    return result;
  };

  uint8_t DetailedTimingDescriptor::type() const {
    return BASE_FAKE_DTD_TYPE;
  };

  void DetailedTimingDescriptor::print(std::ostream& os, uint8_t tabs) const {
    std::string first_level_indent;
    for (int i = 0; i < tabs; ++i)
      first_level_indent.push_back('\t');
    std::string second_level_indent = first_level_indent + "\t";
    os << first_level_indent
      << h_res << 'x'
      << v_res << '@'
      << pixel_clock_hz << " Hz ("
      << h_image_size << " mm x "
      << v_image_size << " mm)\n";

    os << second_level_indent << h_blanking << " blank H pixels\n";
    os << second_level_indent << v_blanking << " blank V lines\n";
    os << second_level_indent << "Hfront " << h_front_porch
      << " Hsync " << h_sync_width
      << " Hborder " << (int)h_border_pixels << '\n';
    os << second_level_indent << "Vfront " << (int)v_front_porch
      << " Vsync " << (int)v_sync_width
      << " Vborder " << (int)v_border_lines << '\n';
  }
}  // namespace Edid
