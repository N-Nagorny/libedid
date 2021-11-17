#include <array>
#include <cstdint>

#include "common.hh"

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
    result[pos++] = pixel_clock & 0xFF;
    result[pos++] = (pixel_clock >> 8) & 0xFF;

    result[pos++] = dtd.h_res & 0xFF;
    result[pos++] = dtd.h_blank_pixels & 0xFF;
    result[pos] = (dtd.h_res >> 4) & 0xF0;
    result[pos++] |= (dtd.h_blank_pixels >> 8) & 0x0F;

    result[pos++] = dtd.v_res & 0xFF;
    result[pos++] = dtd.v_blank_lines & 0xFF;
    result[pos] = (dtd.v_res >> 4) & 0xF0;
    result[pos++] |= (dtd.v_blank_lines >> 8) & 0x0F;

    result[pos++] = dtd.h_sync_offset & 0xFF;
    result[pos++] = dtd.h_sync_width & 0xFF;
    result[pos] = (dtd.v_sync_offset << 4) & 0xF0;
    result[pos++] |= dtd.v_sync_width & 0x0F;
    result[pos] = (dtd.h_sync_offset >> 2) & 0xC0;
    result[pos] |= (dtd.h_sync_width >> 4) & 0x30;
    result[pos] |= (dtd.v_sync_offset >> 2) & 0x0C;
    result[pos++] |= (dtd.v_sync_width >> 4) & 0x03;

    result[pos++] = dtd.h_image_size & 0xFF;
    result[pos++] = dtd.v_image_size & 0xFF;
    result[pos] = (dtd.h_image_size >> 4) & 0xF0;
    result[pos++] |= (dtd.v_image_size >> 8) & 0x0F;

    result[pos++] = dtd.h_border_pixels;
    result[pos++] = dtd.v_border_lines;

    result[pos] = dtd.features_bitmap;

    return result;
  };

  uint8_t calculate_block_checksum(const std::array<uint8_t, EDID_BLOCK_SIZE>& block) {
    int sum = 0;
    for (int i = 0; i < EDID_BLOCK_SIZE - 1; ++i)
      sum += block.at(i);
    return 256 - sum % 256;
  }
}
