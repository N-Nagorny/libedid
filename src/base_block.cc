#include <iostream>

#include "edid/base_block.hh"
#include "edid/exceptions.hh"

namespace Edid {
  bool operator==(const StandardTiming& lhs, const StandardTiming& rhs) {
    return std::tie(lhs.x_resolution, lhs.aspect_ratio, lhs.v_frequency) ==
      std::tie(rhs.x_resolution, rhs.aspect_ratio, rhs.v_frequency);
  }

  bool operator==(const DisplayRangeLimits& lhs, const DisplayRangeLimits& rhs) {
    return std::tie(
      lhs.min_v_rate_hz,
      lhs.max_v_rate_hz,
      lhs.min_h_rate_khz,
      lhs.max_h_rate_khz,
      lhs.max_pixel_clock_rate_mhz,
      lhs.vts
    ) == std::tie(
      rhs.min_v_rate_hz,
      rhs.max_v_rate_hz,
      rhs.min_h_rate_khz,
      rhs.max_h_rate_khz,
      rhs.max_pixel_clock_rate_mhz,
      rhs.vts
    );
  }

  bool operator==(const DisplayName& lhs, const DisplayName& rhs) {
    return lhs.name == rhs.name;
  }

  bool operator==(const DisplaySerialNumber& lhs, const DisplaySerialNumber& rhs) {
    return lhs.display_serial_number == rhs.display_serial_number;
  }

  bool operator==(const DummyDescriptor& lhs, const DummyDescriptor& rhs) {
    return true;
  }

  bool operator==(const BaseBlock& lhs, const BaseBlock& rhs) {
    using namespace details;

    if (lhs.manufacturer_id != rhs.manufacturer_id)
      return false;
    if (lhs.product_code != rhs.product_code)
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
    if (lhs.eighteen_byte_descriptors != rhs.eighteen_byte_descriptors)
      return false;
    return true;
  }

  std::array<uint8_t, EIGHTEEN_BYTES> DisplayRangeLimits::generate_byte_block() const {
    std::array<uint8_t, EIGHTEEN_BYTES> result;
    result.fill(0x0);
    int pos = 0;

    result[pos++] = 0x0;
    result[pos++] = 0x0;
    result[pos++] = 0x0;
    result[pos++] = BASE_DISPLAY_DESCRIPTOR_RANGE_LIMITS_TYPE;

    auto is_value_in_range = [](uint16_t value, uint16_t min = 1, uint16_t max = 510) -> bool {
      return value >= min && value <= max;
    };

    if (!is_value_in_range(min_h_rate_khz))
      throw EdidException("Min H rate is out of range.");
    if (!is_value_in_range(max_h_rate_khz))
      throw EdidException("Max H rate is out of range.");
    if (!is_value_in_range(min_v_rate_hz))
      throw EdidException("Min V rate is out of range.");
    if (!is_value_in_range(max_v_rate_hz))
      throw EdidException("Max V rate is out of range.");
    if (!is_value_in_range(max_pixel_clock_rate_mhz, 10, 2550))
      throw EdidException("Max Pixel Clock rate is out of range.");
    if (max_pixel_clock_rate_mhz % 10 != 0)
      throw EdidException("Max Pixel Clock rate divided by 10 gives a non-zero remainder.");

    uint16_t min_h_rate = min_h_rate_khz;
    uint16_t max_h_rate = max_h_rate_khz;
    uint16_t min_v_rate = min_v_rate_hz;
    uint16_t max_v_rate = max_v_rate_hz;

    if (min_h_rate_khz > 255 && max_h_rate_khz > 255) {
      result[pos] = 0x0C;
      min_h_rate = min_h_rate_khz - 255;
      max_h_rate = max_h_rate_khz - 255;
    } else if (max_h_rate_khz > 255) {
      result[pos] = 0x04;
      max_h_rate = max_h_rate_khz - 255;
    }

    if (min_v_rate_hz > 255 && max_v_rate_hz > 255) {
      result[pos] |= 0x03;
      min_v_rate = min_v_rate_hz - 255;
      max_v_rate = max_v_rate_hz - 255;
    } else if (max_v_rate_hz > 255) {
      result[pos] |= 0x02;
      max_v_rate = max_v_rate_hz - 255;
    }

    ++pos;
    result[pos++] = min_v_rate;
    result[pos++] = max_v_rate;
    result[pos++] = min_h_rate;
    result[pos++] = max_h_rate;
    result[pos++] = max_pixel_clock_rate_mhz / 10;

    result[pos++] = vts;
    result[pos++] = 0x0A;
    for (int i = 0; i < 6; ++i)
      result[pos++] = 0x20;

    return result;
  }

  std::array<uint8_t, EIGHTEEN_BYTES> DisplayName::generate_byte_block() const {
    std::array<uint8_t, EIGHTEEN_BYTES> result;
    result.fill(0x0);
    int pos = 0;

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

    return result;
  }

  std::array<uint8_t, EIGHTEEN_BYTES> DisplaySerialNumber::generate_byte_block() const {
    std::array<uint8_t, EIGHTEEN_BYTES> result;
    result.fill(0x0);
    int pos = 0;

    if (display_serial_number.size() > MAX_DISPLAY_NAME_CHARS)
      throw EdidException("Display Serial Number is more than " + std::to_string(MAX_DISPLAY_NAME_CHARS) + " chars.");
    if (display_serial_number.find(' ') != std::string::npos)
      throw EdidException("Display Serial Number contains spaces.");

    result[pos++] = 0x0;
    result[pos++] = 0x0;
    result[pos++] = 0x0;
    result[pos++] = BASE_DISPLAY_DESCRIPTOR_SERIAL_NUMBER_TYPE;
    result[pos++] = 0x0;

    for (int i = 0; i < display_serial_number.size(); ++i)
      result[pos++] = display_serial_number[i];
    result[pos++] = '\n';
    for (int i = display_serial_number.size(); i < MAX_DISPLAY_NAME_CHARS; ++i)
      result[pos++] = ' ';

    return result;
  }

  std::array<uint8_t, EIGHTEEN_BYTES> DummyDescriptor::generate_byte_block() const {
    std::array<uint8_t, EIGHTEEN_BYTES> result;
    result.fill(0x0);
    int pos = 0;

    result[pos++] = 0x0;
    result[pos++] = 0x0;
    result[pos++] = 0x0;
    result[pos++] = BASE_DISPLAY_DESCRIPTOR_DUMMY_TYPE;
    pos += EIGHTEEN_BYTES - 4;

    return result;
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
    result[pos++] = base_block.product_code & BITMASK_TRUE(8);
    result[pos++] = (base_block.product_code >> 8) & BITMASK_TRUE(8);

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
    for (const auto& eighteen_byte_descriptor : base_block.eighteen_byte_descriptors) {
      auto eighteen_byte_descriptor_visitor =
        [](const auto& descriptor) -> std::array<uint8_t, EIGHTEEN_BYTES> {
          return descriptor.generate_byte_block();
        };

      std::array<uint8_t, EIGHTEEN_BYTES> block =
        eighteen_byte_descriptor.has_value()
        ? std::visit(eighteen_byte_descriptor_visitor, eighteen_byte_descriptor.value())
        : DummyDescriptor().generate_byte_block();

      std::move(block.begin(), block.end(), result.begin() + pos);
      pos += block.size();
    }

    // Extension blocks
    result[pos++] = ext_blocks;

    // Checksum
    result[pos] = calculate_block_checksum(result);

    return result;
  }

  std::optional<StandardTiming> parse_standard_timing(uint8_t byte_1, uint8_t byte_2) {
    std::optional<StandardTiming> result;
    if (byte_1 != 0x01 && byte_2 != 0x01) {
      StandardTiming std_timing;
      std_timing.x_resolution = (byte_1 + 31) * 8;
      std_timing.aspect_ratio = AspectRatio(byte_2 >> 6 & BITMASK_TRUE(2));
      std_timing.v_frequency = (byte_2 & BITMASK_TRUE(6)) + 60;
      result = std_timing;
    }
    return result;
  }

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
    result_struct.product_code = base_block[++pos];
    result_struct.product_code |= base_block[++pos] << 8;

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
      auto std_timing = parse_standard_timing(base_block[pos], base_block[pos + 1]);
      if (std_timing.has_value()) {
        result_struct.standard_timings[std_timing_i++] = std_timing.value();
      }
      pos += 2;
    }

    int eighteen_byte_descriptor_i = 0;
    for (int i = 0; i < BASE_18_BYTE_DESCRIPTORS; ++i) {
      std::optional<EighteenByteDescriptor> descriptor;
      if (base_block[pos] == 0x0 && base_block[pos + 1] == 0x0 && base_block[pos + 2] == 0x0) {
        uint8_t display_descriptor_type = base_block[pos + 3];
        if (display_descriptor_type == BASE_DISPLAY_DESCRIPTOR_RANGE_LIMITS_TYPE) {
          descriptor = DisplayRangeLimits::parse_byte_block(base_block.begin() + pos);
        } else if (display_descriptor_type == BASE_DISPLAY_DESCRIPTOR_NAME_TYPE) {
          descriptor = DisplayName::parse_byte_block(base_block.begin() + pos);
        } else if (display_descriptor_type == BASE_DISPLAY_DESCRIPTOR_SERIAL_NUMBER_TYPE) {
          descriptor = DisplaySerialNumber::parse_byte_block(base_block.begin() + pos);
        } else if (display_descriptor_type == BASE_DISPLAY_DESCRIPTOR_DUMMY_TYPE) {
          descriptor = std::nullopt;
        } else {
          throw EdidException(__FUNCTION__,
            "Display Descriptor at position " + std::to_string(i) +
            " has unknown Display Descriptor Type " + std::to_string(display_descriptor_type));
        }
      } else {
        std::array<uint8_t, EIGHTEEN_BYTES> dtd_binary;
        std::move(
          base_block.begin() + pos,
          base_block.begin() + pos + EIGHTEEN_BYTES,
          dtd_binary.begin()
        );
        descriptor = DetailedTimingDescriptor::parse_byte_block(dtd_binary);
      }
      result_struct.eighteen_byte_descriptors[eighteen_byte_descriptor_i++] = descriptor;
      pos += EIGHTEEN_BYTES;
    }

    result_ext_blocks = base_block[pos];

    return {result_struct, result_ext_blocks};
  }

  void print_standard_timing(std::ostream& os, const StandardTiming& std_timing) {
    os << std_timing.x_resolution << ' '
      << to_string(std_timing.aspect_ratio) << ' '
      << static_cast<int>(std_timing.v_frequency) << " Hz\n";
  }

  void DisplayRangeLimits::print(std::ostream& os, uint8_t tabs) const {
    std::string indent;
    for (int i = 0; i < tabs; ++i)
      indent.push_back('\t');

    os << indent << "Display Range Limits: "
      << min_v_rate_hz << '-'
      << max_v_rate_hz << " Hz V, "
      << min_h_rate_khz << '-'
      << max_h_rate_khz << " kHz H, "
      << max_pixel_clock_rate_mhz << " MHz\n";
  }

  void DisplayName::print(std::ostream& os, uint8_t tabs) const {
    std::string indent;
    for (int i = 0; i < tabs; ++i)
      indent.push_back('\t');

    os << indent << "Display Name: " << name << '\n';
  }

  void DisplaySerialNumber::print(std::ostream& os, uint8_t tabs) const {
    std::string indent;
    for (int i = 0; i < tabs; ++i)
      indent.push_back('\t');

    os << indent << "Display Serial Number: " << display_serial_number << '\n';
  }

  void DummyDescriptor::print(std::ostream& os, uint8_t tabs) const {
    std::string indent;
    for (int i = 0; i < tabs; ++i)
      indent.push_back('\t');

    os << indent << "Dummy Descriptor\n";
  }

  void print_base_block(std::ostream& os, const BaseBlock& base_block) {
    os << "Manufacturer ID: "
      << base_block.manufacturer_id.at(0)
      << base_block.manufacturer_id.at(1)
      << base_block.manufacturer_id.at(2)
      << '\n';
    os << "Product Code: " << base_block.product_code << '\n';
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
        os << '\t';
        print_standard_timing(os, std_timing.value());
      }
    }

    os << "18 Byte Descriptors:\n";
    for (const auto& descriptor : base_block.eighteen_byte_descriptors) {
      if (descriptor.has_value()) {
        std::visit([&os](const auto& d) {
          d.print(os);
        }, descriptor.value());
      }
      else {
        os << '\t' << "Dummy Descriptor\n";
      }
    }


  }
}
