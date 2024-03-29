// Copyright 2023 N-Nagorny
#pragma once

#include <algorithm>
#include <optional>
#include <set>
#include <variant>
#include <vector>

#include "common.hh"
#include "exceptions.hh"

#define CTA861_DATA_BLOCK_HEADER_SIZE 1
#define CTA861_EXTENDED_TAG_SIZE 1

#define CTA861_AUDIO_DATA_BLOCK_TAG 0b001
#define CTA861_VIDEO_DATA_BLOCK_TAG 0b010
#define CTA861_VENDOR_DATA_BLOCK_TAG 0b011
#define CTA861_SPEAKERS_DATA_BLOCK_TAG 0b100
#define CTA861_VESA_DISPLAY_TRANSFER_DATA_BLOCK_TAG 0b101
#define CTA861_EXTENDED_TAG 0b111

#define CTA861_EXTENDED_VIDEO_CAPABILITY_BLOCK_TAG              0x00
#define CTA861_EXTENDED_COLORIMETRY_BLOCK_TAG                   0x05
#define CTA861_EXTENDED_HDR_STATIC_METADATA_BLOCK_TAG           0x06
#define CTA861_EXTENDED_YCBCR420_CAPABILITY_MAP_DATA_BLOCK_TAG  0x0F

#define CTA861_SPEAKERS_DATA_BLOCK_LENGTH 3

namespace Edid {
  static const std::array<uint8_t, 3> hdmi_oui_little_endian = {0x03, 0x0C, 0x00};

  struct CtaDataBlockType {
    uint8_t data_block_type;
    std::optional<uint8_t> extended_tag;
    std::optional<std::array<uint8_t, 3>> oui;

    explicit CtaDataBlockType(uint8_t data_block_type) {
      this->data_block_type = data_block_type;
    }

    explicit CtaDataBlockType(uint8_t data_block_type, uint8_t extended_tag) {
      this->data_block_type = data_block_type;
      this->extended_tag = extended_tag;
    }

    explicit CtaDataBlockType(uint8_t data_block_type, std::array<uint8_t, 3> oui) {
      this->data_block_type = data_block_type;
      this->oui = oui;
    }
  };

#define FIELDS(X) X.data_block_type, X.extended_tag, X.oui
  TIED_COMPARISONS(CtaDataBlockType, FIELDS)
#undef FIELDS

  static auto get_cta_data_block_size = [](const auto& cta_data_block) -> size_t {
    return cta_data_block.size();
  };

  static auto is_vdb_visitor = [](const auto& descriptor) -> bool {
    return descriptor.type() == CtaDataBlockType(CTA861_VIDEO_DATA_BLOCK_TAG);
  };

  static auto is_hdmi_vsdb_visitor = [](const auto& descriptor) -> bool {
    return descriptor.type() == CtaDataBlockType(
      CTA861_VENDOR_DATA_BLOCK_TAG,
      hdmi_oui_little_endian
    );
  };

  struct ICtaDataBlock {
    virtual ~ICtaDataBlock() = default;

    virtual CtaDataBlockType type() const = 0;
    virtual std::vector<uint8_t> generate_byte_block() const = 0;
    virtual void print(std::ostream& os, uint8_t tabs = 1) const = 0;
    virtual size_t size() const = 0;
  };

  struct UnknownDataBlock : ICtaDataBlock {
    std::vector<uint8_t> raw_data;
    uint8_t data_block_tag;
    std::optional<uint8_t> extended_tag;

    UnknownDataBlock() = default;

    // for brace-enclosed initialization despite
    // inheritance from a base class with virtual funcs
    UnknownDataBlock(
      const std::vector<uint8_t>& raw_data,
      uint8_t data_block_tag,
      const std::optional<uint8_t>& extended_tag = std::nullopt
    )
      : raw_data(raw_data)
      , data_block_tag(data_block_tag)
      , extended_tag(extended_tag)
    {}

    CtaDataBlockType type() const override {
      if (extended_tag.has_value()) {
        return CtaDataBlockType(data_block_tag, extended_tag.value());
      }
      return CtaDataBlockType(data_block_tag);
    }

    size_t size() const override {
      return raw_data.size() + CTA861_DATA_BLOCK_HEADER_SIZE + (extended_tag.has_value() ? CTA861_EXTENDED_TAG_SIZE : 0);
    }

    std::vector<uint8_t> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    static UnknownDataBlock parse_byte_block(const uint8_t* start) {
      UnknownDataBlock result;
      int pos = 0;

      result.data_block_tag = *start >> 5 & BITMASK_TRUE(3);
      int data_block_size = *(start + pos++) & BITMASK_TRUE(5);
      if (result.data_block_tag == CTA861_EXTENDED_TAG) {
        result.extended_tag = *(start + pos++);
        --data_block_size;
      }
      result.raw_data = std::vector<uint8_t>(data_block_size, 0x00);

      std::copy(start + pos, start + pos + data_block_size, result.raw_data.begin());
      return result;
    }
  };

#define FIELDS(X) X.raw_data, X.data_block_tag, X.extended_tag
  TIED_COMPARISONS(UnknownDataBlock, FIELDS)
#undef FIELDS

  struct VideoDataBlock : ICtaDataBlock {
    std::vector<uint8_t> vics;

    VideoDataBlock() = default;

    VideoDataBlock(
      const std::vector<uint8_t>& vics
    )
      : vics(vics)
    {}

    size_t size() const override {
      return vics.size() + CTA861_DATA_BLOCK_HEADER_SIZE;
    }

    CtaDataBlockType type() const override {
      return CtaDataBlockType(CTA861_VIDEO_DATA_BLOCK_TAG);
    }

    uint8_t native_vics() const {
      uint8_t result = 0;
      for (uint8_t vic : vics)
        if (vic >= 129 && vic <= 192)
          ++result;
      return result;
    }

    std::vector<uint8_t> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    static VideoDataBlock parse_byte_block(const uint8_t* start) {
      VideoDataBlock result;

      int data_block_tag = *start >> 5 & BITMASK_TRUE(3);
      if (data_block_tag != CTA861_VIDEO_DATA_BLOCK_TAG)
        throw EdidException(__FUNCTION__, "Video Data Block has incorrect Data Block Tag: " +
          std::to_string(data_block_tag)
        );

      const size_t vics = *start & BITMASK_TRUE(5);

      if (vics > 0) {
        result.vics.resize(vics, 0x0);
        std::copy(start + 1, start + 1 + vics, result.vics.data());
      }

      return result;
    }
  };

#define FIELDS(X) X.vics
  TIED_COMPARISONS(VideoDataBlock, FIELDS)
#undef FIELDS

  enum AudioFormatCode {
    RESERVED_0   = 0b0000,
    LPCM         = 0b0001,
    AC3          = 0b0010,
    MPEG1        = 0b0011,
    MP3          = 0b0100,
    MPEG2        = 0b0101,
    AAC          = 0b0110,
    DTS          = 0b0111,
    ATRAC        = 0b1000,
    ONE_BIT      = 0b1001,
    ENHANCED_AC3 = 0b1010,
    DTSHD        = 0b1011,
    MAT          = 0b1100,
    DST          = 0b1101,
    WMA_PRO      = 0b1110,
    RESERVED_15  = 0b1111
  };

  STRINGIFY_ENUM(AudioFormatCode, {
    {RESERVED_0, "RESERVED_0"},
    {LPCM, "L-PCM"},
    {AC3, "AC-3"},
    {MPEG1, "MPEG-1"},
    {MP3, "MP3"},
    {MPEG2, "MPEG-2"},
    {AAC, "AAC LC"},
    {DTS, "DTS"},
    {ATRAC, "ATRAC"},
    {ONE_BIT, "One Bit Audio (Super Audio CD)"},
    {ENHANCED_AC3, "Enhanced AC-3 (a.k.a. Dolby Digital Plus)"},
    {DTSHD, "DTS-HD"},
    {MAT, "Dolby TrueHD (MLP compression, MAT transport)"},
    {DST, "DST"},
    {WMA_PRO, "WMA Pro"},
    {RESERVED_15, "RESERVED_15"},
  })

  enum AudioChannels {
    AC_1 = 0b000,
    AC_2 = 0b001,
    AC_3 = 0b010,
    AC_4 = 0b011,
    AC_5 = 0b100,
    AC_6 = 0b101,
    AC_7 = 0b110,
    AC_8 = 0b111
  };

  STRINGIFY_ENUM(AudioChannels, {
    {AC_1, "1"},
    {AC_2, "2"},
    {AC_3, "3"},
    {AC_4, "4"},
    {AC_5, "5"},
    {AC_6, "6"},
    {AC_7, "7"},
    {AC_8, "8"},
  })

  enum SamplingFrequence {
    SF_RESERVED = 1 << 7,
    SF_192 = 1 << 6,
    SF_176 = 1 << 5,
    SF_96 = 1 << 4,
    SF_88 = 1 << 3,
    SF_48 = 1 << 2,
    SF_44_1 = 1 << 1,
    SF_32 = 1 << 0
  };

  STRINGIFY_ENUM(SamplingFrequence, {
    {SF_RESERVED, "SF_RESERVED"},
    {SF_192, "192 kHz"},
    {SF_176, "176 kHz"},
    {SF_96, "96 kHz"},
    {SF_88, "88 kHz"},
    {SF_48, "48 kHz"},
    {SF_44_1, "44.1 kHz"},
    {SF_32, "32 kHz"},
  })

  using SamplingFrequences = uint8_t;

  enum LpcmBitDepth {
    LPCM_BD_24 = 1 << 2,
    LPCM_BD_20 = 1 << 1,
    LPCM_BD_16 = 1 << 0
  };

  STRINGIFY_ENUM(LpcmBitDepth, {
    {LPCM_BD_24, "LPCM_BD_24"},
    {LPCM_BD_20, "LPCM_BD_20"},
    {LPCM_BD_16, "LPCM_BD_16"},
  })

  using LpcmBitDepths = uint8_t;

  struct ShortAudioDescriptor {
    AudioFormatCode audio_format = AudioFormatCode::RESERVED_0;
    AudioChannels channels = AudioChannels::AC_1;
    SamplingFrequences sampling_freqs = 0;
    LpcmBitDepths lpcm_bit_depths = 0;

    static constexpr uint8_t size() {
      return 3;
    }
  };

#define FIELDS(X) X.audio_format, X.channels, X.sampling_freqs, X.lpcm_bit_depths
  TIED_COMPARISONS(ShortAudioDescriptor, FIELDS)
#undef FIELDS

  struct AudioDataBlock : ICtaDataBlock {
    std::vector<ShortAudioDescriptor> sads;

    size_t size() const override {
      return sads.size() * ShortAudioDescriptor::size() + CTA861_DATA_BLOCK_HEADER_SIZE;
    }

    CtaDataBlockType type() const override {
      return CtaDataBlockType(CTA861_AUDIO_DATA_BLOCK_TAG);
    }

    std::vector<uint8_t> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    static AudioDataBlock parse_byte_block(const uint8_t* start) {
      AudioDataBlock result;
      int pos = 0;

      int data_block_tag = *start >> 5 & BITMASK_TRUE(3);
      if (data_block_tag != CTA861_AUDIO_DATA_BLOCK_TAG)
        throw EdidException(__FUNCTION__, "Audio Data Block has incorrect Data Block Tag: " +
          std::to_string(data_block_tag)
        );

      int sads = *start & BITMASK_TRUE(5);

      if (sads % ShortAudioDescriptor::size() != 0)
        throw EdidException(__FUNCTION__, "Audio Data Block has size which is not "
          "a factor of " + std::to_string(ShortAudioDescriptor::size()) +
          ": " + std::to_string(sads));
      else
        sads /= ShortAudioDescriptor::size();

      result.sads.resize(sads);

      for (int i = 0; i < sads; ++i) {
        ShortAudioDescriptor sad;
        sad.audio_format = AudioFormatCode(*(start + ++pos) >> 3 & BITMASK_TRUE(4));
        sad.channels = AudioChannels(*(start + pos) & BITMASK_TRUE(3));
        sad.sampling_freqs = *(start + ++pos);
        sad.lpcm_bit_depths = *(start + ++pos);
        result.sads[i] = sad;
      }

      return result;
    }
  };

#define FIELDS(X) X.sads
  TIED_COMPARISONS(AudioDataBlock, FIELDS)
#undef FIELDS

  enum Speaker {
    RESERVED                    = 1 << 7,
    REAR_LEFT_AND_RIGHT_CENTER  = 1 << 6,
    FRONT_LEFT_AND_RIGHT_CENTER = 1 << 5,
    REAR_CENTER                 = 1 << 4,
    REAR_LEFT_AND_RIGHT         = 1 << 3,
    FRONT_CENTER                = 1 << 2,
    LFE                         = 1 << 1,
    FRONT_LEFT_AND_RIGHT        = 1 << 0
  };

  STRINGIFY_ENUM(Speaker, {
    {RESERVED, "RESERVED"},
    {REAR_LEFT_AND_RIGHT_CENTER, "REAR_LEFT_AND_RIGHT_CENTER"},
    {FRONT_LEFT_AND_RIGHT_CENTER, "FRONT_LEFT_AND_RIGHT_CENTER"},
    {REAR_CENTER, "REAR_CENTER"},
    {REAR_LEFT_AND_RIGHT, "REAR_LEFT_AND_RIGHT"},
    {FRONT_CENTER, "FRONT_CENTER"},
    {LFE, "LFE"},
    {FRONT_LEFT_AND_RIGHT, "FRONT_LEFT_AND_RIGHT"},
  })

  using SpeakerAllocation = uint8_t;

  struct SpeakerAllocationDataBlock : ICtaDataBlock {
    SpeakerAllocation speaker_allocation = 0;

    size_t size() const override {
      return CTA861_SPEAKERS_DATA_BLOCK_LENGTH + CTA861_DATA_BLOCK_HEADER_SIZE;
    }

    CtaDataBlockType type() const override {
      return CtaDataBlockType(CTA861_SPEAKERS_DATA_BLOCK_TAG);
    }

    std::vector<uint8_t> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    static SpeakerAllocationDataBlock parse_byte_block(const uint8_t* start) {
      SpeakerAllocationDataBlock result;
      int pos = 0;

      int data_block_tag = *start >> 5 & BITMASK_TRUE(3);
      if (data_block_tag != CTA861_SPEAKERS_DATA_BLOCK_TAG)
        throw EdidException(__FUNCTION__, "Speaker Allocation Data Block has incorrect Data Block Tag: " +
          std::to_string(data_block_tag)
        );

      int data_block_size = *start & BITMASK_TRUE(5);

      if (data_block_size != CTA861_SPEAKERS_DATA_BLOCK_LENGTH)
        throw EdidException(__FUNCTION__, "Speaker Allocation Data Block has invalid length: " +
          std::to_string(data_block_size));

      result.speaker_allocation = *(start + ++pos);

      return result;
    }
  };

#define FIELDS(X) X.speaker_allocation
  TIED_COMPARISONS(SpeakerAllocationDataBlock, FIELDS)
#undef FIELDS

  struct YCbCr420CapabilityMapDataBlock : ICtaDataBlock {
    std::set<uint8_t> svd_indices;

    size_t size() const override {
      if (!svd_indices.size())
        return CTA861_DATA_BLOCK_HEADER_SIZE + CTA861_EXTENDED_TAG_SIZE;
      auto last_element = svd_indices.rbegin();
      const size_t bytes = *last_element / 8 + (*last_element % 8 == 0 ? 0 : 1);
      return bytes + CTA861_DATA_BLOCK_HEADER_SIZE + CTA861_EXTENDED_TAG_SIZE;
    }

    CtaDataBlockType type() const override {
      return CtaDataBlockType(CTA861_EXTENDED_TAG, CTA861_EXTENDED_YCBCR420_CAPABILITY_MAP_DATA_BLOCK_TAG);
    }

    std::vector<uint8_t> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    static YCbCr420CapabilityMapDataBlock parse_byte_block(const uint8_t* start) {
      YCbCr420CapabilityMapDataBlock result;

      int data_block_tag = *start >> 5 & BITMASK_TRUE(3);
      if (data_block_tag != CTA861_EXTENDED_TAG)
        throw EdidException(__FUNCTION__, "Extended Tag Data Block has incorrect Data Block Tag: " +
          std::to_string(data_block_tag)
        );

      const int length = *start++ & BITMASK_TRUE(5);

      int extended_tag = *start++;
      if (extended_tag != CTA861_EXTENDED_YCBCR420_CAPABILITY_MAP_DATA_BLOCK_TAG)
        throw EdidException(__FUNCTION__, "YCbCr420CapabilityMapDataBlock has incorrect Extended Data Block Tag: " +
          std::to_string(extended_tag)
        );

      uint8_t svd_index = 1;
      for (int i = 0; i < length - 1; ++i) {
        uint8_t byte = *(start + i);
        for (int j = 0; j < 8; ++j) {
          if (byte & BITMASK_TRUE(1) << j) {
            result.svd_indices.insert(svd_index + j);
          }
        }
        svd_index += 8;
      }

      return result;
    }
  };

#define FIELDS(X) X.svd_indices
  TIED_COMPARISONS(YCbCr420CapabilityMapDataBlock, FIELDS)
#undef FIELDS

  // ---------- CTA-861-I Section 7.5.5 ----------

  enum ColorimetryStandard {
    CS_XV_YCC601   = 1 << 0,
    CS_XV_YCC709   = 1 << 1,
    CS_S_YCC601    = 1 << 2,
    CS_OP_YCC601   = 1 << 3,
    CS_OP_RGB      = 1 << 4,
    CS_BT2020_CYCC = 1 << 5,
    CS_BT2020_YCC  = 1 << 6,
    CS_BT2020_RGB  = 1 << 7,

    CS_DEFAULT_RGB  = 1 << 12,
    CS_S_RGB        = 1 << 13,
    CS_ICTCP        = 1 << 14,
    CS_ST2113_RGB   = 1 << 15
  };

  STRINGIFY_ENUM(ColorimetryStandard, {
    {CS_XV_YCC601,    "xvYCC601"},
    {CS_XV_YCC709,    "xvYCC709"},
    {CS_S_YCC601,     "sYCC601"},
    {CS_OP_YCC601,    "opYCC601"},
    {CS_OP_RGB,       "opRGB"},
    {CS_BT2020_CYCC,  "BT2020cYCC"},
    {CS_BT2020_YCC,   "BT2020YCC"},
    {CS_BT2020_RGB,   "BT2020RGB"},
    {CS_DEFAULT_RGB,  "defaultRGB"},
    {CS_S_RGB,        "sRGB"},
    {CS_ICTCP,        "ICtCp"},
    {CS_ST2113_RGB,   "ST2113RGB"},
  })

  enum GamutMetadataProfile {
    GMP_0 = 1 << 0,
    GMP_1 = 1 << 1,
    GMP_2 = 1 << 2,
    GMP_3 = 1 << 3
  };

  STRINGIFY_ENUM(GamutMetadataProfile, {
    {GMP_0, "MD0"},
    {GMP_1, "MD1"},
    {GMP_2, "MD2"},
    {GMP_3, "MD3"},
  })

  struct ColorimetryDataBlock : ICtaDataBlock {
    uint16_t colorimetry_standards = 0;
    uint8_t gamut_metadata_profiles = 0;

    ColorimetryDataBlock() = default;

    // for brace-enclosed initialization despite
    // inheritance from a base class with virtual funcs
    ColorimetryDataBlock(
      uint16_t colorimetry_standards,
      uint8_t gamut_metadata_profiles
    )
      : colorimetry_standards(colorimetry_standards)
      , gamut_metadata_profiles(gamut_metadata_profiles)
    {}

    size_t size() const override {
      const size_t payload_size = 2;
      return payload_size + CTA861_DATA_BLOCK_HEADER_SIZE + CTA861_EXTENDED_TAG_SIZE;
    }

    CtaDataBlockType type() const override {
      return CtaDataBlockType(CTA861_EXTENDED_TAG, CTA861_EXTENDED_COLORIMETRY_BLOCK_TAG);
    }

    std::vector<uint8_t> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    static ColorimetryDataBlock parse_byte_block(const uint8_t* iter) {
      ColorimetryDataBlock result;

      const uint8_t data_block_tag = *iter++ >> 5 & BITMASK_TRUE(3);
      if (data_block_tag != CTA861_EXTENDED_TAG)
        throw EdidException(__FUNCTION__, "Extended Tag Data Block has incorrect Data Block Tag: " +
          std::to_string(data_block_tag)
        );

      int extended_tag = *iter++;
      if (extended_tag != CTA861_EXTENDED_COLORIMETRY_BLOCK_TAG)
        throw EdidException(__FUNCTION__, "Colorimetry Data Block has incorrect Extended Data Block Tag: " +
          std::to_string(extended_tag)
        );

      result.colorimetry_standards = *iter++;
      result.colorimetry_standards |= (*iter >> 4 & BITMASK_TRUE(4)) << 12;
      result.gamut_metadata_profiles = *iter & BITMASK_TRUE(4);

      return result;
    }
  };

#define FIELDS(X) X.colorimetry_standards, X.gamut_metadata_profiles
  TIED_COMPARISONS(ColorimetryDataBlock, FIELDS)
#undef FIELDS

  // ---------- CTA-861-I Section 7.5.13 ----------

  enum ElectroOpticalTransferFunction {
    TF_SDR = 1 << 0,
    TF_HDR = 1 << 1,
    TF_PQ  = 1 << 2,
    TF_HLG = 1 << 3,
    TF_4   = 1 << 4,
    TF_5   = 1 << 5,
  };

  STRINGIFY_ENUM(ElectroOpticalTransferFunction, {
    {TF_SDR, "SDR"},
    {TF_HDR, "HDR"},
    {TF_PQ,  "PQ"},
    {TF_HLG, "HLG"},
    {TF_4,   "TF_4"},
    {TF_5,   "TF_5"}
  })

  enum StaticMetadataType {
    SM_TYPE_1 = 1 << 0,
    SM_1      = 1 << 1,
    SM_2      = 1 << 2,
    SM_3      = 1 << 3,
    SM_4      = 1 << 4,
    SM_5      = 1 << 5,
    SM_6      = 1 << 6,
    SM_7      = 1 << 7,
  };

  STRINGIFY_ENUM(StaticMetadataType, {
    {SM_TYPE_1, "StaticMetadataType1"},
    {SM_1,      "SM_1"},
    {SM_2,      "SM_2"},
    {SM_3,      "SM_3"},
    {SM_4,      "SM_4"},
    {SM_5,      "SM_5"},
    {SM_6,      "SM_6"},
    {SM_7,      "SM_7"}
  })

  struct HdrStaticMetadataDataBlock : ICtaDataBlock {
    uint8_t transfer_functions = 0;
    uint8_t static_metadata_types = 0;
    std::optional<uint8_t> max_luminance_code_value;
    std::optional<uint8_t> max_frame_average_luminance_code_value;
    std::optional<uint8_t> min_luminance_code_value;

    HdrStaticMetadataDataBlock() = default;

    // for brace-enclosed initialization despite
    // inheritance from a base class with virtual funcs
    HdrStaticMetadataDataBlock(
      uint8_t transfer_functions,
      uint8_t static_metadata_types,
      const std::optional<uint8_t>& max_luminance_code_value = std::nullopt,
      const std::optional<uint8_t>& max_frame_average_luminance_code_value = std::nullopt,
      const std::optional<uint8_t>& min_luminance_code_value = std::nullopt
    )
      : transfer_functions(transfer_functions)
      , static_metadata_types(static_metadata_types)
      , max_luminance_code_value(max_luminance_code_value)
      , max_frame_average_luminance_code_value(max_frame_average_luminance_code_value)
      , min_luminance_code_value(min_luminance_code_value)
    {}

    size_t size() const override {
      size_t payload_size = 2;
      if (min_luminance_code_value.has_value()) {
        payload_size += 3;
      }
      else if (max_frame_average_luminance_code_value.has_value()) {
        payload_size += 2;
      }
      else if (max_luminance_code_value.has_value()) {
        payload_size += 1;
      }
      return payload_size + CTA861_DATA_BLOCK_HEADER_SIZE + CTA861_EXTENDED_TAG_SIZE;
    }

    CtaDataBlockType type() const override {
      return CtaDataBlockType(CTA861_EXTENDED_TAG, CTA861_EXTENDED_HDR_STATIC_METADATA_BLOCK_TAG);
    }

    std::vector<uint8_t> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;
    static HdrStaticMetadataDataBlock parse_byte_block(const uint8_t* iter);
  };

#define FIELDS(X) X.transfer_functions, X.static_metadata_types, \
                  X.max_luminance_code_value, X.max_frame_average_luminance_code_value, \
                  X.min_luminance_code_value
  TIED_COMPARISONS(HdrStaticMetadataDataBlock, FIELDS)
#undef FIELDS

  // ---------- CTA-861-I Section 7.5.6 ----------

  enum OverUnderscanSupport {
    OUB_NO_DATA = 0b00,
    OUB_OVER    = 0b01,
    OUB_UNDER   = 0b10,
    OUB_BOTH    = 0b11
  };

  STRINGIFY_ENUM(OverUnderscanSupport, {
    {OUB_NO_DATA, "no_data"},
    {OUB_OVER,    "always_overscanned"},
    {OUB_UNDER,   "always_underscanned"},
    {OUB_BOTH,    "supports_over_and_underscan"}
  })

  struct VideoCapabilityDataBlock : ICtaDataBlock {
    bool is_ycc_quantization_range_selectable = false;
    bool is_rgb_quantization_range_selectable = true;
    OverUnderscanSupport pt_scan_behaviour = OUB_NO_DATA;  // Preferred Timings
    OverUnderscanSupport it_scan_behaviour = OUB_NO_DATA;  // Information Technologies
    OverUnderscanSupport ce_scan_behaviour = OUB_NO_DATA;  // Consumer Electronics

    VideoCapabilityDataBlock() = default;

    // for brace-enclosed initialization despite
    // inheritance from a base class with virtual funcs
    VideoCapabilityDataBlock(
      bool is_ycc_quantization_range_selectable,
      bool is_rgb_quantization_range_selectable,
      OverUnderscanSupport pt_scan_behaviour,
      OverUnderscanSupport it_scan_behaviour,
      OverUnderscanSupport ce_scan_behaviour
    )
      : is_ycc_quantization_range_selectable(is_ycc_quantization_range_selectable)
      , is_rgb_quantization_range_selectable(is_rgb_quantization_range_selectable)
      , pt_scan_behaviour(pt_scan_behaviour)
      , it_scan_behaviour(it_scan_behaviour)
      , ce_scan_behaviour(ce_scan_behaviour)
    {}

    constexpr size_t size() const override {
      return 1 + CTA861_DATA_BLOCK_HEADER_SIZE + CTA861_EXTENDED_TAG_SIZE;
    }

    CtaDataBlockType type() const override {
      return CtaDataBlockType(CTA861_EXTENDED_TAG, CTA861_EXTENDED_VIDEO_CAPABILITY_BLOCK_TAG);
    }

    std::vector<uint8_t> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;
    static VideoCapabilityDataBlock parse_byte_block(const uint8_t* iter);
  };

#define FIELDS(X) X.is_ycc_quantization_range_selectable, X.is_rgb_quantization_range_selectable, \
                  X.pt_scan_behaviour, X.it_scan_behaviour, X.ce_scan_behaviour
  TIED_COMPARISONS(VideoCapabilityDataBlock, FIELDS)
#undef FIELDS
}  // namespace Edid
