#include <array>
#include <cstdint>

#include "edid/common.hh"

namespace Edid {
  bool operator==(const DetailedTimingDescriptor& lhs, const DetailedTimingDescriptor& rhs) {
    return std::tie(
      lhs.pixel_clock_hz,
      lhs.h_res,
      lhs.v_res,
      lhs.h_blank_pixels,
      lhs.v_blank_lines,
      lhs.h_sync_offset,
      lhs.v_sync_offset,
      lhs.h_sync_width,
      lhs.v_sync_width,
      lhs.h_image_size,
      lhs.v_image_size,
      lhs.h_border_pixels,
      lhs.v_border_lines,
      lhs.features_bitmap
    ) == std::tie(
      rhs.pixel_clock_hz,
      rhs.h_res,
      rhs.v_res,
      rhs.h_blank_pixels,
      rhs.v_blank_lines,
      rhs.h_sync_offset,
      rhs.v_sync_offset,
      rhs.h_sync_width,
      rhs.v_sync_width,
      rhs.h_image_size,
      rhs.v_image_size,
      rhs.h_border_pixels,
      rhs.v_border_lines,
      rhs.features_bitmap
    );
  }

  bool operator!=(const DetailedTimingDescriptor& lhs, const DetailedTimingDescriptor& rhs) {
    return !(lhs == rhs);
  }

  std::array<uint8_t, DTD_BLOCK_SIZE> make_dtd(const DetailedTimingDescriptor& dtd) {
    std::array<uint8_t, DTD_BLOCK_SIZE> result;
    result.fill(0x0);
    int pos = 0;

    uint16_t pixel_clock = dtd.pixel_clock_hz / 10'000;
    result[pos++] = pixel_clock & BITMASK_TRUE(8);
    result[pos++] = (pixel_clock >> 8) & BITMASK_TRUE(8);

    result[pos++] = dtd.h_res & BITMASK_TRUE(8);
    result[pos++] = dtd.h_blank_pixels & BITMASK_TRUE(8);
    result[pos] = ((dtd.h_res >> 8) & BITMASK_TRUE(4)) << 4;
    result[pos++] |= (dtd.h_blank_pixels >> 8) & BITMASK_TRUE(4);

    result[pos++] = dtd.v_res & BITMASK_TRUE(8);
    result[pos++] = dtd.v_blank_lines & BITMASK_TRUE(8);
    result[pos] = ((dtd.v_res >> 8) & BITMASK_TRUE(4)) << 4;
    result[pos++] |= (dtd.v_blank_lines >> 8) & BITMASK_TRUE(4);

    result[pos++] = dtd.h_sync_offset & BITMASK_TRUE(8);
    result[pos++] = dtd.h_sync_width & BITMASK_TRUE(8);
    result[pos] = (dtd.v_sync_offset & BITMASK_TRUE(4)) << 4;
    result[pos++] |= dtd.v_sync_width & BITMASK_TRUE(4);
    result[pos] = ((dtd.h_sync_offset >> 8) & BITMASK_TRUE(2)) << 6;
    result[pos] |= ((dtd.h_sync_width >> 8) & BITMASK_TRUE(2)) << 4;
    result[pos] |= ((dtd.v_sync_offset >> 4) & BITMASK_TRUE(2)) << 2;
    result[pos++] |= ((dtd.v_sync_width >> 4) & BITMASK_TRUE(2)) << 0;

    result[pos++] = dtd.h_image_size & BITMASK_TRUE(8);
    result[pos++] = dtd.v_image_size & BITMASK_TRUE(8);
    result[pos] = ((dtd.h_image_size >> 8) & BITMASK_TRUE(4)) << 4;
    result[pos++] |= (dtd.v_image_size >> 8) & BITMASK_TRUE(4);

    result[pos++] = dtd.h_border_pixels;
    result[pos++] = dtd.v_border_lines;

    result[pos] = dtd.features_bitmap;

    return result;
  };

  DetailedTimingDescriptor parse_dtd(const std::array<uint8_t, DTD_BLOCK_SIZE>& dtd) {
    DetailedTimingDescriptor result;
    int pos = 0;

    result.pixel_clock_hz = dtd[pos++];
    result.pixel_clock_hz |= (dtd[pos++] & BITMASK_TRUE(8)) << 8;
    result.pixel_clock_hz *= 10'000;

    result.h_res = dtd[pos++];
    result.h_blank_pixels = dtd[pos++];
    result.h_res |= (dtd[pos] >> 4 & BITMASK_TRUE(4)) << 8;
    result.h_blank_pixels |= (dtd[pos++] & BITMASK_TRUE(4)) << 8;

    result.v_res = dtd[pos++];
    result.v_blank_lines = dtd[pos++];
    result.v_res |= (dtd[pos] >> 4 & BITMASK_TRUE(4)) << 8;
    result.v_blank_lines |= (dtd[pos++] & BITMASK_TRUE(4)) << 8;

    result.h_sync_offset = dtd[pos++];
    result.h_sync_width = dtd[pos++];
    result.v_sync_offset = dtd[pos] >> 4 & BITMASK_TRUE(4);
    result.v_sync_width = dtd[pos++] & BITMASK_TRUE(4);
    result.h_sync_offset |= (dtd[pos] >> 6 & BITMASK_TRUE(2)) << 8;
    result.h_sync_width |= (dtd[pos] >> 4 & BITMASK_TRUE(2)) << 8;
    result.v_sync_offset |= (dtd[pos] >> 2 & BITMASK_TRUE(2)) << 4;
    result.v_sync_width |= (dtd[pos++] & BITMASK_TRUE(2)) << 4;

    result.h_image_size = dtd[pos++];
    result.v_image_size = dtd[pos++];
    result.h_image_size |= (dtd[pos] >> 4 & BITMASK_TRUE(4)) << 8;
    result.v_image_size |= (dtd[pos++] & BITMASK_TRUE(4)) << 8;

    result.h_border_pixels = dtd[pos++];
    result.v_border_lines = dtd[pos++];

    result.features_bitmap = dtd[pos];
    return result;
  };

  void print_detailed_timing_descriptor(std::ostream& os, const DetailedTimingDescriptor& detailed_timing_descriptor, int tabs) {
    std::string first_level_indent;
    for (int i = 0; i < tabs; ++i)
      first_level_indent.push_back('\t');
    std::string second_level_indent = first_level_indent + "\t";
    os << first_level_indent
      << detailed_timing_descriptor.h_res << 'x'
      << detailed_timing_descriptor.v_res << '@'
      << detailed_timing_descriptor.pixel_clock_hz << " Hz ("
      << detailed_timing_descriptor.h_image_size << " mm x "
      << detailed_timing_descriptor.v_image_size << " mm)\n";

    os << second_level_indent << detailed_timing_descriptor.h_blank_pixels << " blank H pixels\n";
    os << second_level_indent << detailed_timing_descriptor.v_blank_lines << " blank V lines\n";
    os << second_level_indent << "Hfront " << detailed_timing_descriptor.h_sync_offset
      << " Hsync " << detailed_timing_descriptor.h_sync_width
      << " Hborder " << (int)detailed_timing_descriptor.h_border_pixels << '\n';
    os << second_level_indent << "Vfront " << (int)detailed_timing_descriptor.v_sync_offset
      << " Vsync " << (int)detailed_timing_descriptor.v_sync_width
      << " Vborder " << (int)detailed_timing_descriptor.v_border_lines << '\n';
  }

  uint8_t calculate_block_checksum(const std::array<uint8_t, EDID_BLOCK_SIZE>& block) {
    int sum = 0;
    for (int i = 0; i < EDID_BLOCK_SIZE - 1; ++i)
      sum += block.at(i);
    return 256 - sum % 256;
  }
}
