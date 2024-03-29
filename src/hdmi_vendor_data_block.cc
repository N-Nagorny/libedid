// Copyright 2023 N-Nagorny
#include "edid/hdmi_vendor_data_block.hh"

namespace Edid {
  std::vector<uint8_t> HdmiVendorDataBlock::generate_byte_block() const {
    if (size() - CTA861_DATA_BLOCK_HEADER_SIZE > 31)
      throw EdidException("HDMI VDB is longer than 31 bytes.");

    std::vector<uint8_t> result(size(), 0x0);
    int pos = 0;

    result[pos] = CTA861_VENDOR_DATA_BLOCK_TAG << 5;
    result[pos++] |= (size() - CTA861_DATA_BLOCK_HEADER_SIZE) & BITMASK_TRUE(5);

    result[pos++] = hdmi_oui_little_endian.at(0);
    result[pos++] = hdmi_oui_little_endian.at(1);
    result[pos++] = hdmi_oui_little_endian.at(2);

    const auto& source_pa_bytes = source_phy_addr;
    result[pos] = (source_pa_bytes.at(0) & BITMASK_TRUE(4)) << 4;
    result[pos++] |= source_pa_bytes.at(1) & BITMASK_TRUE(4);
    result[pos] = (source_pa_bytes.at(2) & BITMASK_TRUE(4)) << 4;
    result[pos++] |= source_pa_bytes.at(3) & BITMASK_TRUE(4);

    HdmiVendorDataBlockCapabilities caps = capabilities.has_value()
      ? capabilities.value()
      : 0;
    uint16_t max_tmds_clock_mhz_ = max_tmds_clock_mhz.has_value()
      ? max_tmds_clock_mhz.value() / 5
      : 0;

    if (bytes_6_8_exist()) {
      result[pos++] = caps;

      result[pos++] = max_tmds_clock_mhz_ & BITMASK_TRUE(8);

      result[pos] = (latency.has_value() ? 1 : 0) << 7;
      result[pos] |= (interlaced_latency.has_value() ? 1 : 0) << 6;
      result[pos] |= (hdmi_video_bytes_size() > 0 ? 1 : 0) << 5;
      result[pos++] |= content_types & BITMASK_TRUE(4);

      if (latency.has_value()) {
        result[pos++] = latency->first;
        result[pos++] = latency->second;
      }

      if (interlaced_latency.has_value()) {
        result[pos++] = interlaced_latency->first;
        result[pos++] = interlaced_latency->second;
      }

      if (hdmi_video.has_value()) {
        // byte (13) in Table 8-16
        bool has_video_support = hdmi_video->stereo_video_support.has_value();
        result[pos] = has_video_support << 7;
        if (has_video_support) {
          if (hdmi_video->stereo_video_support->vics.has_value()) {
            printf("hdmi_video->stereo_video_support->vics.has_value()\n");
            result[pos] |= 0b10 << 5;
          }
          else if (hdmi_video->stereo_video_support->formats.has_value()) {
            result[pos] |= 0b01 << 5;
          }
        }
        result[pos++] |= hdmi_video->image_size_meaning & BITMASK_TRUE(2) << 3;

        result[pos] = (hdmi_video->hdmi_vics.size() & BITMASK_TRUE(3)) << 5;
        result[pos++] |= stereo_video_info_size() & BITMASK_TRUE(5);


        for (uint8_t vic : hdmi_video->hdmi_vics) {
          result[pos++] = vic;
        }

        if (hdmi_video->stereo_video_support.has_value()) {
          if (hdmi_video->stereo_video_support->formats.has_value()) {
            result[pos++] = hdmi_video->stereo_video_support->formats->first;
            result[pos++] = hdmi_video->stereo_video_support->formats->second;
          }
          if (hdmi_video->stereo_video_support->vics.has_value()) {
            result[pos++] = (hdmi_video->stereo_video_support->vics.value() >> 8) & BITMASK_TRUE(8);
            result[pos++] = hdmi_video->stereo_video_support->vics.value() & BITMASK_TRUE(8);
          }
        }

        for (const auto& vic_info : hdmi_video->vic_3d_support) {
          result[pos] = (vic_info.vic_index & BITMASK_TRUE(4)) << 4;
          result[pos++] |= vic_info.format & BITMASK_TRUE(4);

          if (vic_info.subsampling_3d.has_value()) {
            result[pos++] = (vic_info.subsampling_3d.value() & BITMASK_TRUE(4)) << 4;
          }
        }
      }
    }
    else if (max_tmds_clock_mhz.has_value()) {
      result[pos++] = caps;

      result[pos++] = max_tmds_clock_mhz_ & BITMASK_TRUE(8);
    }
    else if (capabilities.has_value()) {
      result[pos++] = caps;
    }

    return result;
  }

  // TODO(N-Nagorny)
  void HdmiVendorDataBlock::print(std::ostream& os, uint8_t tabs) const {
    os << "Vendor-Specific Data Block (HDMI), OUI 00-0C-03: " << '\n';
    os << '\t' << "Source physical address: "
      << static_cast<int>(source_phy_addr[0]) << "."
      << static_cast<int>(source_phy_addr[1]) << "."
      << static_cast<int>(source_phy_addr[2]) << "."
      << static_cast<int>(source_phy_addr[3]) << "." << '\n';
    os << '\n';
  }
}  // namespace Edid
