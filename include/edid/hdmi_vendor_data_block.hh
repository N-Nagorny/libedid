#pragma once

#include <optional>
#include <vector>

#include "common.hh"
#include "exceptions.hh"
#include "cta_data_block.hh"

#define IEEE_OUI_SIZE 3

namespace Edid {
  enum ImageSizeMeaning {
    ISM_NO_INFO             = 0b00,
    ISM_ASPECT_RATIO        = 0b01,
    ISM_SIZES_ROUNDED_TO_CM = 0b10,
    ISM_SIZES_DIVIDED_BY_5  = 0b11
  };

  STRINGIFY_ENUM(ImageSizeMeaning, {
    {ISM_NO_INFO, "ISM_NO_INFO"},
    {ISM_ASPECT_RATIO, "ISM_ASPECT_RATIO"},
    {ISM_SIZES_ROUNDED_TO_CM, "ISM_SIZES_ROUNDED_TO_CM"},
    {ISM_SIZES_DIVIDED_BY_5, "ISM_SIZES_DIVIDED_BY_5"},
  })

  enum HdmiVendorDataBlockByte6Flags {
    HVDBB6F_SUPPORTS_AI = 1 << 7,
    HVDBB6F_DC_48BIT = 1 << 6,
    HVDBB6F_DC_36BIT = 1 << 5,
    HVDBB6F_DC_30BIT = 1 << 4,
    HVDBB6F_DC_Y444 = 1 << 3,
    HVDBB6F_RESERVED_0 = 1 << 2,
    HVDBB6F_RESERVED_1 = 1 << 1,
    HVDBB6F_DVI_DUAL = 1 << 0
  };

  STRINGIFY_ENUM(HdmiVendorDataBlockByte6Flags, {
    {HVDBB6F_SUPPORTS_AI, "HVDBB6F_SUPPORTS_AI"},
    {HVDBB6F_DC_48BIT, "HVDBB6F_DC_48BIT"},
    {HVDBB6F_DC_36BIT, "HVDBB6F_DC_36BIT"},
    {HVDBB6F_DC_30BIT, "HVDBB6F_DC_30BIT"},
    {HVDBB6F_DC_Y444, "HVDBB6F_DC_Y444"},
    {HVDBB6F_RESERVED_0, "HVDBB6F_RESERVED_0"},
    {HVDBB6F_RESERVED_1, "HVDBB6F_RESERVED_1"},
    {HVDBB6F_DVI_DUAL, "HVDBB6F_DVI_DUAL"},
  })

  enum ContentType {
    CT_GAME = 1 << 3,
    CT_CINEMA = 1 << 2,
    CT_PHOTO = 1 << 1,
    CT_GRAPHICS = 1 << 0
  };

  STRINGIFY_ENUM(ContentType, {
    {CT_GAME, "CT_GAME"},
    {CT_CINEMA, "CT_CINEMA"},
    {CT_PHOTO, "CT_PHOTO"},
    {CT_GRAPHICS, "CT_GRAPHICS"},
  })

  // 3D_Structure_ALL_7...0 in HDMI 1.4b
  enum StereoVideoFormatByte2 {
    SVF_TOP_AND_BOTTOM = 1 << 6,
    SVF_L_DEPTH_GRAPHICS = 1 << 5,
    SVF_L_DEPTH = 1 << 4,
    SVF_SIDE_BY_SIDE_FULL = 1 << 3,
    SVF_LINE_ALTERNATIVE = 1 << 2,
    SVF_FIELD_ALTERNATIVE = 1 << 1,
    SVF_FRAME_PACKING = 1 << 0
  };

  STRINGIFY_ENUM(StereoVideoFormatByte2, {
    {SVF_TOP_AND_BOTTOM, "Top-and-Bottom"},
    {SVF_L_DEPTH_GRAPHICS, "L + depth + graphics + graphics-depth"},
    {SVF_L_DEPTH, "L + depth"},
    {SVF_SIDE_BY_SIDE_FULL, "Side-by-Side (Full)"},
    {SVF_LINE_ALTERNATIVE, "Line alternative"},
    {SVF_FIELD_ALTERNATIVE, "Field alternative"},
    {SVF_FRAME_PACKING, "Frame packing"}
  })

  // 3D_Structure_ALL_15...8 in HDMI 1.4b
  enum StereoVideoFormatByte1 {
    SVF_SIDE_BY_SIDE_QUINCUNX = 1 << 7,
    SVF_SIDE_BY_SIDE_HORIZONTAL = 1 << 0
  };
  STRINGIFY_ENUM(StereoVideoFormatByte1, {
    {SVF_SIDE_BY_SIDE_QUINCUNX,"Horizontal"},
    {SVF_SIDE_BY_SIDE_HORIZONTAL,"Four quincunx matrix"}
  })


  using HdmiVendorDataBlockCapabilities = uint8_t;
  using ContentTypes = uint8_t;

  struct StereoVideoSupport {
    std::optional<std::pair<uint8_t, uint8_t>> formats; // 3D_Structure_ALL_15...0
    std::optional<uint16_t> vics; // 3D_MASK_ALL_15..0
  };

  bool operator==(const StereoVideoSupport& lhs, const StereoVideoSupport& rhs);

  // 3D_Structure in HDMI 1.4b
  enum StereoVideoTransmissionFormat {
    SVTF_FRAME_PACKING = 0b0000,
    SVTF_FIELD_ALTERNATIVE = 0b0001,
    SVTF_LINE_ALTERNATIVE = 0b0010,
    SVTF_SIDE_BY_SIDE_FULL = 0b0011,
    SVTF_L_DEPTH = 0b0100,
    SVTF_L_DEPTH_GRAPHICS = 0b0101,
    SVTF_TOP_AND_BOTTOM = 0b0110,
    SVTF_SIDE_BY_SIDE_HALF = 0b1000
  };

  STRINGIFY_ENUM(StereoVideoTransmissionFormat, {
    {SVTF_FRAME_PACKING,     "Frame packing"},
    {SVTF_FIELD_ALTERNATIVE, "Field alternative"},
    {SVTF_LINE_ALTERNATIVE,  "Line alternative"},
    {SVTF_SIDE_BY_SIDE_FULL, "Side-by-Side (Full)"},
    {SVTF_L_DEPTH,           "L + depth"},
    {SVTF_L_DEPTH_GRAPHICS,  "L + depth + graphics + graphics-depth"},
    {SVTF_TOP_AND_BOTTOM,    "Top-and-Bottom"},
    {SVTF_SIDE_BY_SIDE_HALF, "Side-by-Side (Half)"},
  })

  // 3D_Detail_X for Side-by-Side (Half) in HDMI 1.4b
  enum StereoVideoSubsampling {
    SVS_HORIZ_AND_QUINCUNX   = 0b0000,
    SVS_HORIZONTAL           = 0b0001,
    SVS_QUINCUNX             = 0b0110,
    SVS_ODD_LEFT_ODD_RIGHT   = 0b0111,
    SVS_ODD_LEFT_EVEN_RIGHT  = 0b1000,
    SVS_EVEN_LEFT_ODD_RIGHT  = 0b1001,
    SVS_EVEN_LEFT_EVEN_RIGHT = 0b1010,
  };

  STRINGIFY_ENUM(StereoVideoSubsampling, {
    {SVS_HORIZ_AND_QUINCUNX,   "Horizontal and four quincunx matrix"},
    {SVS_HORIZONTAL,           "Horizontal"},
    {SVS_QUINCUNX,             "Four quincunx matrix"},
    {SVS_ODD_LEFT_ODD_RIGHT,   "Odd/Left, Odd/Right"},
    {SVS_ODD_LEFT_EVEN_RIGHT,  "Odd/Left, Even/Right"},
    {SVS_EVEN_LEFT_ODD_RIGHT,  "Even/Left, Odd/Right"},
    {SVS_EVEN_LEFT_EVEN_RIGHT, "Even/Left, Even/Right"},
  })

  struct Vic3dSupport {
    uint8_t vic_index; // 0..15
    StereoVideoTransmissionFormat format;
    std::optional<StereoVideoSubsampling> subsampling_3d;
  };

  bool operator==(const Vic3dSupport& lhs, const Vic3dSupport& rhs);

  struct HdmiVideoSubblock {
    ImageSizeMeaning image_size_meaning = ISM_NO_INFO;
    std::vector<uint8_t> hdmi_vics;
    std::optional<StereoVideoSupport> stereo_video_support;
    std::vector<Vic3dSupport> vic_3d_support;
  };

  bool operator==(const HdmiVideoSubblock& lhs, const HdmiVideoSubblock& rhs);

  struct HdmiVendorDataBlock {
    std::array<uint8_t, 4> source_phy_addr;
    std::optional<HdmiVendorDataBlockCapabilities> capabilities;
    std::optional<uint16_t> max_tmds_clock_mhz; // Max is 1280 MHz
    ContentTypes content_types = 0;
    std::optional<std::pair<uint8_t, uint8_t>> latency;
    std::optional<std::pair<uint8_t, uint8_t>> interlaced_latency;
    std::optional<HdmiVideoSubblock> hdmi_video;

    size_t size() const {
      return CTA861_DATA_BLOCK_HEADER_SIZE +
        IEEE_OUI_SIZE +
        payload_size();
    }

    CtaDataBlockType type() const {
      return CtaDataBlockType(
        CTA861_VENDOR_DATA_BLOCK_TAG,
        hdmi_oui_little_endian
      );
    }

    uint8_t stereo_video_info_size() const {
      if (!hdmi_video.has_value()) {
        return 0;
      }

      uint8_t result = 0;


      for (const auto& vic_3d_item : hdmi_video->vic_3d_support) {
        result +=
          vic_3d_item.subsampling_3d.has_value()
            ? 2
            : 1;
      }

      if (hdmi_video->stereo_video_support.has_value()) {
        result += hdmi_video->stereo_video_support->formats.has_value() ? 2 : 0;
        result += hdmi_video->stereo_video_support->vics.has_value() ? 2 : 0;
      }

      return result;
    }

    uint8_t hdmi_video_bytes_size() const {
      if (!hdmi_video.has_value()) {
        return 0;
      }
      return 2 + stereo_video_info_size() + hdmi_video->hdmi_vics.size();
    }

    bool bytes_6_8_exist() const {
      if (hdmi_video_bytes_size() > 0)
        return true;
      if (latency.has_value() || interlaced_latency.has_value()) {
        return true;
      }
      if (content_types != 0) {
        return true;
      }
      return false;
    }

    uint8_t payload_size() const {
      uint8_t result = 2; // Source Physical Address

      if (bytes_6_8_exist()) {
        result += 3;

        if (latency.has_value()) {
          result += 2;
        }

        if (interlaced_latency.has_value()) {
          result += 2;
        }

        result += hdmi_video_bytes_size();
      }
      else if (max_tmds_clock_mhz.has_value()) {
        result += 2;
      }
      else if (capabilities.has_value()) {
        result += 1;
      }

      return result;
    }

    std::vector<uint8_t> generate_byte_block() const;
    void print(std::ostream& os, uint8_t tabs = 1) const;

    template<typename Iterator>
    static HdmiVendorDataBlock parse_byte_block(Iterator start) {
      HdmiVendorDataBlock result;

      // byte 8
      bool latency_fields_present = false;
      bool i_latency_fields_present = false;
      bool hdmi_video_present = false;

      // byte (13)
      bool stereo_video_formats_present = false;
      bool stereo_video_vics_present = false;
      int hdmi_vic_len = 0;
      int hdmi_3d_len = 0;

      int data_block_tag = *start >> 5 & BITMASK_TRUE(3);
      if (data_block_tag != CTA861_VENDOR_DATA_BLOCK_TAG)
        throw EdidException(__FUNCTION__, "Vendor-specific Data Block has incorrect Data Block Tag: " +
          std::to_string(data_block_tag)
        );

      const int length = *start++ & BITMASK_TRUE(5);

      if (length < 5 || length == 9)
        throw EdidException(__FUNCTION__, "Vendor-specific Data Block has incorrect value for length (the minimum is 5; 9 is impossible): " +
          std::to_string(length)
        );

      std::array<uint8_t, 3> oui;
      oui[0] = *start++;
      oui[1] = *start++;
      oui[2] = *start++;

      if (oui != hdmi_oui_little_endian)
        throw EdidException(__FUNCTION__, "HdmiVendorDataBlock has incorrect OUI: " +
          oui.at(0) + oui.at(1) + oui.at(2)
        );

      result.source_phy_addr[0] = *start >> 4 & BITMASK_TRUE(4);
      result.source_phy_addr[1] = *start++ & BITMASK_TRUE(4);
      result.source_phy_addr[2] = *start >> 4 & BITMASK_TRUE(4);
      result.source_phy_addr[3] = *start++ & BITMASK_TRUE(4);

      if (length >= 6) {
        result.capabilities = *start++;
      }

      if (length >= 7) {
        result.max_tmds_clock_mhz = *start++ * 5;
      }

      if (length >= 8) {
        uint8_t byte_8 = *start++;
        result.content_types = byte_8 & BITMASK_TRUE(4);

        latency_fields_present = byte_8 >> 7 & BITMASK_TRUE(1);
        i_latency_fields_present = byte_8 >> 6 & BITMASK_TRUE(1);
        hdmi_video_present = byte_8 >> 5 & BITMASK_TRUE(1);
      }

      if (latency_fields_present) {
        if (length < 10)
          throw EdidException(__FUNCTION__, "Vendor-specific Data Block has incorrect value for length (latency_fields_present): " +
            std::to_string(length)
          );

        result.latency = {*start, *(start + 1)};
        start += 2;
      }

      if (i_latency_fields_present) {
        if (length < 10)
          throw EdidException(__FUNCTION__, "Vendor-specific Data Block has incorrect value for length (i_latency_fields_present): " +
            std::to_string(length)
          );

        result.interlaced_latency = {*start, *(start + 1)};
        start += 2;
      }

      if (hdmi_video_present) {
        if (length < 10)
          throw EdidException(__FUNCTION__, "Vendor-specific Data Block has incorrect value for length (hdmi_video_present): " +
            std::to_string(length)
          );

        HdmiVideoSubblock subblock;
        uint8_t byte_13 = *start++;
        uint8_t byte_14 = *start++;

        if (byte_13 >> 7 & BITMASK_TRUE(1)) {
          subblock.stereo_video_support = StereoVideoSupport{};
        }

        uint8_t stereo_video_multi_present = byte_13 >> 5 & BITMASK_TRUE(2);
        if (stereo_video_multi_present == 0b01) {
          stereo_video_formats_present = true;
        }
        else if (stereo_video_multi_present == 0b10) {
          stereo_video_formats_present = true;
          stereo_video_vics_present = true;
        }
        subblock.image_size_meaning = ImageSizeMeaning(byte_13 >> 3 & BITMASK_TRUE(2));

        hdmi_vic_len = byte_14 >> 5 & BITMASK_TRUE(3);
        hdmi_3d_len = byte_14 & BITMASK_TRUE(5);

        subblock.hdmi_vics.resize(hdmi_vic_len);

        for (int i = 0; i < hdmi_vic_len; i++) {
          subblock.hdmi_vics[i] = *start++;
        }

        if (stereo_video_formats_present) {
          subblock.stereo_video_support->formats = {*start, *(start + 1)};
          start += 2;
          hdmi_3d_len -= 2;
        }

        if (stereo_video_vics_present) {
          uint16_t byte = *start++ << 8;
          byte |= *start++;
          subblock.stereo_video_support->vics = byte;
          hdmi_3d_len -= 2;
        }

        std::vector<Vic3dSupport> vic_3d_support;
        while (hdmi_3d_len > 0) {
          Vic3dSupport vic_3d_item;
          vic_3d_item.vic_index = *start >> 4 & BITMASK_TRUE(4);
          uint8_t stereo_video_structure_x = *start++ & BITMASK_TRUE(4);
          vic_3d_item.format = StereoVideoTransmissionFormat(stereo_video_structure_x);
          if (stereo_video_structure_x >= 0b1000 && stereo_video_structure_x <= 0b1111) {
            vic_3d_item.subsampling_3d = StereoVideoSubsampling(*start++ >> 4 & BITMASK_TRUE(4));
          }
          vic_3d_support.push_back(vic_3d_item);
          hdmi_3d_len--;

        }
        if (!vic_3d_support.empty()) {
          subblock.vic_3d_support = std::move(vic_3d_support);
        }

        result.hdmi_video = subblock;
      }

      return result;
    }
  };

  bool operator==(const HdmiVendorDataBlock& lhs, const HdmiVendorDataBlock& rhs);
}
