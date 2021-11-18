#include <iostream>

#include "base_block.hh"
#include "exceptions.hh"

namespace Edid {
  bool operator==(const StandardTiming& lhs, const StandardTiming& rhs) {
    return std::tie(lhs.x_resolution, lhs.aspect_ratio, lhs.v_frequency) ==
      std::tie(rhs.x_resolution, rhs.aspect_ratio, rhs.v_frequency);
  }

  bool operator!=(const StandardTiming& lhs, const StandardTiming& rhs) {
    return !(lhs == rhs);
  }

  bool operator==(const DisplayRangeLimits& lhs, const DisplayRangeLimits& rhs) {
    return std::tie(lhs.min_v_rate_hz, lhs.max_v_rate_hz, lhs.min_h_rate_khz, lhs.max_h_rate_khz, lhs.max_pixel_clock_rate_mhz) ==
      std::tie(rhs.min_v_rate_hz, rhs.max_v_rate_hz, rhs.min_h_rate_khz, rhs.max_h_rate_khz, rhs.max_pixel_clock_rate_mhz);
  }

  bool operator!=(const DisplayRangeLimits& lhs, const DisplayRangeLimits& rhs) {
    return !(lhs == rhs);
  }

  bool operator==(const BaseBlock& lhs, const BaseBlock& rhs) {
    if (lhs.manufacturer_id != rhs.manufacturer_id)
      return false;
    if (lhs.manufacturer_product_code != rhs.manufacturer_product_code)
      return false;
    if (lhs.serial_number != rhs.serial_number)
      return false;
    if (lhs.manufacture_week != rhs.manufacture_week)
      return false;
    if (lhs.manufacture_year != rhs.manufacture_year)
      return false;
    if (lhs.edid_major_version != rhs.edid_major_version)
      return false;
    if (lhs.edid_minor_version != rhs.edid_minor_version)
      return false;
    if (lhs.bits_per_color != rhs.bits_per_color)
      return false;
    if (lhs.video_interface != rhs.video_interface)
      return false;
    if (lhs.h_screen_size != rhs.h_screen_size)
      return false;
    if (lhs.v_screen_size != rhs.v_screen_size)
      return false;
    if (lhs.gamma != rhs.gamma)
      return false;
    if (lhs.dpms_standby != rhs.dpms_standby)
      return false;
    if (lhs.dpms_suspend != rhs.dpms_suspend)
      return false;
    if (lhs.dpms_active_off != rhs.dpms_active_off)
      return false;
    if (lhs.display_type != rhs.display_type)
      return false;
    if (lhs.standard_srgb != rhs.standard_srgb)
      return false;
    if (lhs.preferred_timing_mode != rhs.preferred_timing_mode)
      return false;
    if (lhs.continuous_timings != rhs.continuous_timings)
      return false;
    if (lhs.chromaticity != rhs.chromaticity)
      return false;
    if (lhs.established_timings_1 != rhs.established_timings_1)
      return false;
    if (lhs.established_timings_2 != rhs.established_timings_2)
      return false;
    if (lhs.established_timings_3 != rhs.established_timings_3)
      return false;
    if (lhs.standard_timings != rhs.standard_timings)
      return false;
    if (lhs.detailed_timing_descriptors != rhs.detailed_timing_descriptors)
      return false;
    if (lhs.display_range_limits != rhs.display_range_limits)
      return false;
    if (lhs.display_name != rhs.display_name)
      return false;
    return true;
  }

  std::array<uint8_t, EDID_BLOCK_SIZE> generate_base_block(const BaseBlock& base_block, uint8_t ext_blocks) {
    std::array<uint8_t, EDID_BLOCK_SIZE> result;
    result.fill(0x0);
    int pos = 0;

    // EDID header
    for (uint8_t byte : base_block_header)
      result[pos++] = byte;

    // Manufacturer ID
    auto ascii_char_to_edid = [](char ch) -> char {
      return (ch - 64) & BITMASK_TRUE(5);
    };

    result[pos] = ascii_char_to_edid(base_block.manufacturer_id.at(0)) << 2;

    result[pos++] |= (ascii_char_to_edid(base_block.manufacturer_id.at(1)) >> 3);
    result[pos] = (ascii_char_to_edid(base_block.manufacturer_id.at(1)) & BITMASK_TRUE(3)) << 5;

    result[pos++] |= ascii_char_to_edid(base_block.manufacturer_id.at(2));

    // Product code
    result[pos++] = base_block.manufacturer_product_code & BITMASK_TRUE(8);
    result[pos++] = (base_block.manufacturer_product_code >> 8) & BITMASK_TRUE(8);

    // Serial number
    result[pos++] = base_block.serial_number & BITMASK_TRUE(8);
    result[pos++] = (base_block.serial_number >> 8) & BITMASK_TRUE(8);
    result[pos++] = (base_block.serial_number >> 16) & BITMASK_TRUE(8);
    result[pos++] = (base_block.serial_number >> 24) & BITMASK_TRUE(8);

    // Week of manufacture
    result[pos++] = base_block.manufacture_week;

    // Year of manufacture
    result[pos++] = base_block.manufacture_year - BASE_START_MANUFACTURE_YEAR;

    // EDID version
    result[pos++] = base_block.edid_major_version;
    result[pos++] = base_block.edid_minor_version;

    // Basic display parameters
    result[pos] = 1 << 7; // Digital input
    result[pos] |= (base_block.bits_per_color << 4);
    result[pos++] |= base_block.video_interface;

    result[pos++] = base_block.h_screen_size;
    result[pos++] = base_block.v_screen_size;

    result[pos++] = base_block.gamma;

    result[pos] = base_block.dpms_standby << 7;
    result[pos] |= base_block.dpms_suspend << 6;
    result[pos] |= base_block.dpms_active_off << 5;
    result[pos] |= base_block.display_type << 3;
    result[pos] |= base_block.standard_srgb << 2;
    result[pos] |= base_block.preferred_timing_mode << 1;
    result[pos++] |= base_block.continuous_timings;

    // Chromaticity coordinates
    for (uint8_t byte : base_block.chromaticity)
      result[pos++] = byte;

    // Established Timings
    result[pos++] = base_block.established_timings_1;
    result[pos++] = base_block.established_timings_2;
    result[pos++] = base_block.established_timings_3;

    // Standard Timings
    for (const std::optional<StandardTiming>& std_timing : base_block.standard_timings) {
      if (std_timing.has_value()) {
        result[pos++] = std_timing->x_resolution / 8 - 31;
        result[pos] = std_timing->aspect_ratio << 6;
        result[pos++] |= (std_timing->v_frequency - 60) & BITMASK_TRUE(6);
      } else {
        result[pos++] = 0x01;
        result[pos++] = 0x01;
      }
    }

    // 18 byte descriptors
    auto mark_as_dummy = [&result, &pos]() {
      result[pos++] = 0x0;
      result[pos++] = 0x0;
      result[pos++] = 0x0;
      result[pos++] = BASE_DISPLAY_DESCRIPTOR_DUMMY_TYPE;
      pos += DTD_BLOCK_SIZE - 4;
    };

    auto insert_display_range_limits = [&result, &pos](DisplayRangeLimits limits) {
      result[pos++] = 0x0;
      result[pos++] = 0x0;
      result[pos++] = 0x0;
      result[pos++] = BASE_DISPLAY_DESCRIPTOR_RANGE_LIMITS_TYPE;

      auto is_value_in_range = [](uint16_t value, uint16_t min = 1, uint16_t max = 510) -> bool {
        return value >= min && value <= max;
      };

      if (!is_value_in_range(limits.min_h_rate_khz))
        throw EdidException("Min H rate is out of range.");
      if (!is_value_in_range(limits.max_h_rate_khz))
        throw EdidException("Max H rate is out of range.");
      if (!is_value_in_range(limits.min_v_rate_hz))
        throw EdidException("Min V rate is out of range.");
      if (!is_value_in_range(limits.max_v_rate_hz))
        throw EdidException("Max V rate is out of range.");
      if (!is_value_in_range(limits.max_pixel_clock_rate_mhz, 10, 2550))
        throw EdidException("Max Pixel Clock rate is out of range.");
      if (limits.max_pixel_clock_rate_mhz % 10 != 0)
        throw EdidException("Max Pixel Clock rate divided by 10 gives a non-zero remainder.");

      if (limits.min_h_rate_khz > 255 && limits.max_h_rate_khz > 255) {
        result[pos] = 0x0C;
        limits.min_h_rate_khz -= 255;
        limits.max_h_rate_khz -= 255;
      } else if (limits.max_h_rate_khz > 255) {
        result[pos] = 0x04;
        limits.max_h_rate_khz -= 255;
      }

      if (limits.min_v_rate_hz > 255 && limits.max_v_rate_hz > 255) {
        result[pos] |= 0x03;
        limits.min_v_rate_hz -= 255;
        limits.max_v_rate_hz -= 255;
      } else if (limits.max_v_rate_hz > 255) {
        result[pos] |= 0x02;
        limits.max_v_rate_hz -= 255;
      }

      ++pos;
      result[pos++] = limits.min_v_rate_hz;
      result[pos++] = limits.max_v_rate_hz;
      result[pos++] = limits.min_h_rate_khz;
      result[pos++] = limits.max_h_rate_khz;
      result[pos++] = limits.max_pixel_clock_rate_mhz / 10;

      result[pos++] = 0x01;
      result[pos++] = 0x0A;
      for (int i = 0; i < 6; ++i)
        result[pos++] = 0x20;
    };

    auto insert_display_name = [&result, &pos](const std::string& name) {
      if (name.size() > MAX_DISPLAY_NAME_CHARS)
        throw EdidException("Display Name is more than " + std::to_string(MAX_DISPLAY_NAME_CHARS) + " chars.");
      if (name.find(' ') != std::string::npos)
        throw EdidException("Display Name contains spaces.");

      result[pos++] = 0x0;
      result[pos++] = 0x0;
      result[pos++] = 0x0;
      result[pos++] = BASE_DISPLAY_DESCRIPTOR_NAME_TYPE;
      result[pos++] = 0x0;

      for (int i = 0; i < name.size(); ++i)
        result[pos++] = name[i];
      result[pos++] = '\n';
      for (int i = name.size(); i < MAX_DISPLAY_NAME_CHARS; ++i)
        result[pos++] = ' ';
    };

    for (const auto& detailed_timing_descriptor : base_block.detailed_timing_descriptors) {
      if (detailed_timing_descriptor.has_value()) {
        auto dtd = make_dtd(detailed_timing_descriptor.value());
        std::move(dtd.begin(), dtd.end(), result.begin() + pos);
        pos += dtd.size();
      } else {
        mark_as_dummy();
      }
    }

    if (base_block.display_range_limits.has_value()) {
      insert_display_range_limits(base_block.display_range_limits.value());
    } else {
      mark_as_dummy();
    }

    if (base_block.display_name.has_value()) {
      insert_display_name(base_block.display_name.value());
    } else {
      mark_as_dummy();
    }

    // Extension blocks
    result[pos++] = ext_blocks;

    // Checksum
    result[pos] = calculate_block_checksum(result);

    return result;
  }

  // TODO: Add DTD parsing
  std::pair<BaseBlock, uint8_t> parse_base_block(const std::array<uint8_t, EDID_BLOCK_SIZE>& base_block) {
    BaseBlock result_struct;
    uint8_t result_ext_blocks = 0;
    int pos = 0;

    // EDID header
    for (uint8_t byte : base_block_header)
      if (base_block[pos++] != byte)
        throw EdidException(__FUNCTION__, "EDID base block header is invalid.");

    // Checksum
    if (base_block[EDID_BLOCK_SIZE - 1] != calculate_block_checksum(base_block))
      throw EdidException(__FUNCTION__, "EDID base block checksum is invalid.");

    // Manufacturer ID
    auto edid_char_to_ascii = [](char ch) -> char {
      return ch + 64;
    };

    result_struct.manufacturer_id[0] = edid_char_to_ascii(base_block[pos] >> 2);
    result_struct.manufacturer_id[1] = edid_char_to_ascii(
      ((base_block[pos] & BITMASK_TRUE(2)) << 3) | ((base_block[pos + 1] >> 5) & BITMASK_TRUE(3))
    );
    result_struct.manufacturer_id[2] = edid_char_to_ascii(base_block[++pos] & BITMASK_TRUE(5));

    // Product code
    result_struct.manufacturer_product_code = base_block[++pos];
    result_struct.manufacturer_product_code |= base_block[++pos] << 8;

    // Serial number
    result_struct.serial_number = base_block[++pos];
    result_struct.serial_number |= base_block[++pos] << 8;
    result_struct.serial_number |= base_block[++pos] << 16;
    result_struct.serial_number |= base_block[++pos] << 24;

    // Week of manufacture
    result_struct.manufacture_week = base_block[++pos];

    // Year of manufacture
    result_struct.manufacture_year = base_block[++pos] + BASE_START_MANUFACTURE_YEAR;

    // EDID version
    result_struct.edid_major_version = base_block[++pos];
    result_struct.edid_minor_version = base_block[++pos];

    // Basic display parameters
    result_struct.bits_per_color = BitDepth((base_block[++pos] >> 4) & BITMASK_TRUE(3));
    result_struct.video_interface = VideoInterface(base_block[pos] & BITMASK_TRUE(4));

    result_struct.h_screen_size = base_block[++pos];
    result_struct.v_screen_size = base_block[++pos];

    result_struct.gamma = base_block[++pos];

    result_struct.dpms_standby = base_block[++pos] & 1 << 7;
    result_struct.dpms_suspend = base_block[pos] & 1 << 6;
    result_struct.dpms_active_off = base_block[pos] & 1 << 5;
    result_struct.display_type = DigitalDisplayType(base_block[pos] >> 3 & BITMASK_TRUE(2));
    result_struct.standard_srgb = base_block[pos] & 1 << 2;
    result_struct.preferred_timing_mode = base_block[pos] & 1 << 1;
    result_struct.continuous_timings = base_block[pos] & 1;

    // Chromaticity coordinates
    for (uint8_t& byte : result_struct.chromaticity)
      byte = base_block[++pos];

    // Established Timings
    result_struct.established_timings_1 = base_block[++pos];
    result_struct.established_timings_2 = base_block[++pos];
    result_struct.established_timings_3 = base_block[++pos];

    // Standard Timings
    int std_timing_i = 0;
    ++pos;
    for (int i = 0; i < result_struct.standard_timings.size(); ++i) {
      if (base_block[pos] != 0x01 && base_block[pos + 1] != 0x01) {
        StandardTiming std_timing;
        std_timing.x_resolution = (base_block[pos] + 31) * 8;
        std_timing.aspect_ratio = AspectRatio(base_block[pos + 1] >> 6 & BITMASK_TRUE(2));
        std_timing.v_frequency = (base_block[pos + 1] & BITMASK_TRUE(6)) + 60;
        result_struct.standard_timings[std_timing_i++] = std_timing;
      }
      pos += 2;
    }

    // 18 byte descriptors
    auto parse_display_range_limits = [&base_block](int descriptor_num) -> DisplayRangeLimits {
      int pos = BASE_18_BYTE_DESCRIPTORS_OFFSET + descriptor_num * 18;
      DisplayRangeLimits result;
      bool add_255_to_h_max = false;
      bool add_255_to_h_min = false;
      bool add_255_to_v_max = false;
      bool add_255_to_v_min = false;
      pos += BASE_DISPLAY_DESCRIPTOR_HEADER_SIZE - 1; // Only for Display Range Limits Descriptor
      if (base_block[pos] >> 2 & BITMASK_TRUE(2) == 0b10) {
        add_255_to_h_max = true;
      } else if (base_block[pos] >> 2 & BITMASK_TRUE(2) == 0b11) {
        add_255_to_h_max = true;
        add_255_to_h_min = true;
      }
      if (base_block[pos] & BITMASK_TRUE(2) == 0b10) {
        add_255_to_v_max = true;
      } else if (base_block[pos] & BITMASK_TRUE(2) == 0b11) {
        add_255_to_v_max = true;
        add_255_to_v_min = true;
      }
      result.min_v_rate_hz = base_block[++pos];
      result.max_v_rate_hz = base_block[++pos];
      result.min_h_rate_khz = base_block[++pos];
      result.max_h_rate_khz = base_block[++pos];
      if (add_255_to_v_min)
        result.min_v_rate_hz += 255;
      if (add_255_to_v_max)
        result.max_v_rate_hz += 255;
      if (add_255_to_h_min)
        result.min_h_rate_khz += 255;
      if (add_255_to_h_max)
        result.max_h_rate_khz += 255;
      result.max_pixel_clock_rate_mhz = base_block[++pos] * 10;
      return result;
    };

    auto parse_display_name = [&base_block](int descriptor_num) -> std::string {
      int pos = BASE_18_BYTE_DESCRIPTORS_OFFSET + descriptor_num * 18;
      std::string result;
      pos += BASE_DISPLAY_DESCRIPTOR_HEADER_SIZE;
      for (int i = 0; i < MAX_DISPLAY_NAME_CHARS + 1; ++i) {
        if (base_block[pos + i] != ' ' && base_block[pos + i] != '\n')
          result.push_back(base_block[pos + i]);
      }
      return result;
    };

    int dtd_timing_i = 0;
    for (int i = 0; i < BASE_18_BYTE_DESCRIPTORS; ++i) {
      if (base_block[pos] == 0x0 && base_block[pos + 1] == 0x0 && base_block[pos + 2] == 0x0) {
        if (base_block[pos + 3] == BASE_DISPLAY_DESCRIPTOR_RANGE_LIMITS_TYPE) {
          result_struct.display_range_limits = parse_display_range_limits(i);
        } else if (base_block[pos + 3] == BASE_DISPLAY_DESCRIPTOR_NAME_TYPE) {
          result_struct.display_name = parse_display_name(i);
        } else if (base_block[pos + 3] != BASE_DISPLAY_DESCRIPTOR_DUMMY_TYPE) {
          throw EdidException(__FUNCTION__,
            "Display Descriptor at position " + std::to_string(i) +
            " has unknown Display Descriptor Type " + std::to_string(static_cast<int>(base_block[pos + 4])));
        }
      } else {
        std::array<uint8_t, DTD_BLOCK_SIZE> dtd_binary;
        std::move(
          base_block.begin() + pos,
          base_block.begin() + pos + DTD_BLOCK_SIZE,
          dtd_binary.begin()
        );
        result_struct.detailed_timing_descriptors[dtd_timing_i++] = parse_dtd(dtd_binary);
      }
      pos += DTD_BLOCK_SIZE;
    }

    return {result_struct, result_ext_blocks};
  }

  void print_base_block(std::ostream& os, const BaseBlock& base_block) {
    os << "Manufacturer ID: "
      << base_block.manufacturer_id.at(0)
      << base_block.manufacturer_id.at(1)
      << base_block.manufacturer_id.at(2)
      << '\n';
    os << "Manufacturer Product Code: " << base_block.manufacturer_product_code << '\n';
    os << "Serial Number: " << base_block.serial_number << '\n';
    os << "Manufacture Week: " << static_cast<int>(base_block.manufacture_week) << '\n';
    os << "Manufacture Year: " << base_block.manufacture_year << '\n';
    os << "EDID version: "
      << static_cast<int>(base_block.edid_major_version)
      << '.'
      << static_cast<int>(base_block.edid_minor_version)
      << '\n';
    os << "Bits per primary color: " << to_string(base_block.bits_per_color) << '\n';
    os << "Video interface: " << to_string(base_block.video_interface) << '\n';
    if (base_block.h_screen_size == 0 && base_block.v_screen_size == 0)
      os << "Image size is undefined (variable)\n";
    else
      os << "Maximum image size: "
        << static_cast<int>(base_block.h_screen_size) << " cm"
        << " x "
        << static_cast<int>(base_block.v_screen_size) << " cm"
        << '\n';
    os << "Gamma: " << (base_block.gamma + 100) / 100.0 << '\n';
    os << "DPMS standby support: " << base_block.dpms_standby << '\n';
    os << "DPMS suspend support: " << base_block.dpms_suspend << '\n';
    os << "DPMS active-off support: " << base_block.dpms_active_off << '\n';
    os << "Digital display type: " << to_string(base_block.display_type) << '\n';
    os << "Standard sRGB colour space: " << base_block.standard_srgb << '\n';
    os << "The first DTD is Preferred: " << base_block.preferred_timing_mode << '\n';
    os << "Continuous timings with GTF and CVT: " << base_block.continuous_timings << '\n';

    os << "Chromaticity block: ";
    for (uint8_t byte : base_block.chromaticity)
      os << static_cast<int>(byte) << ' ';
    os << '\n';

    os << "Established Timings I & II:\n";
    for (EstablishedTiming1 et : bitfield_to_enums<EstablishedTiming1>(base_block.established_timings_1))
      os << '\t' << to_string(et) << '\n';
    for (EstablishedTiming2 et : bitfield_to_enums<EstablishedTiming2>(base_block.established_timings_2))
      os << '\t' << to_string(et) << '\n';
    for (EstablishedTiming3 et : bitfield_to_enums<EstablishedTiming3>(base_block.established_timings_3))
      os << '\t' << to_string(et) << '\n';
    os << '\n';

    os << "Standard Timings:\n";
    for (const std::optional<StandardTiming>& std_timing : base_block.standard_timings) {
      if (std_timing.has_value()) {
        os << '\t'
          << std_timing->x_resolution << ' '
          << to_string(std_timing->aspect_ratio) << ' '
          << static_cast<int>(std_timing->v_frequency) << " Hz\n";
      }
    }

    os << "Detailed Timing Descriptors:\n";
    for (const auto& detailed_timing_descriptor : base_block.detailed_timing_descriptors) {
      if (detailed_timing_descriptor.has_value()) {
        os << '\t'
          << detailed_timing_descriptor->h_res << 'x'
          << detailed_timing_descriptor->v_res << '@'
          << detailed_timing_descriptor->pixel_clock_hz << " Hz\n";
      }
    }

    if (base_block.display_range_limits.has_value()) {
      os << "Display Range Limits: "
        << base_block.display_range_limits->min_v_rate_hz << '-'
        << base_block.display_range_limits->max_v_rate_hz << " Hz V, "
        << base_block.display_range_limits->min_h_rate_khz << '-'
        << base_block.display_range_limits->max_h_rate_khz << " kHz H, "
        << base_block.display_range_limits->max_pixel_clock_rate_mhz << " MHz\n";
    }

    if (base_block.display_name.has_value()) {
      os << "Display Name: " << base_block.display_name.value() << '\n';
    }
  }
}
