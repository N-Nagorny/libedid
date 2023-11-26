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

#define CTA861_EXTENDED_YCBCR420_CAPABILITY_MAP_DATA_BLOCK_TAG 15

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


  struct UnknownDataBlock {
    std::vector<uint8_t> raw_data;
    uint8_t data_block_tag;
    std::optional<uint8_t> extended_tag;

    CtaDataBlockType type() const {
      if (extended_tag.has_value()) {
        return CtaDataBlockType(data_block_tag, extended_tag.value());
      }
      return CtaDataBlockType(data_block_tag);
    }

    size_t size() const {
      return raw_data.size() + CTA861_DATA_BLOCK_HEADER_SIZE + (extended_tag.has_value() ? CTA861_EXTENDED_TAG_SIZE : 0);
    }

    std::vector<uint8_t> generate_byte_block() const;
    void print(std::ostream& os, uint8_t tabs = 1) const;

    template<typename Iterator>
    static UnknownDataBlock parse_byte_block(Iterator start) {
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

  bool operator==(const UnknownDataBlock& lhs, const UnknownDataBlock& rhs);

  struct VideoDataBlock {
    std::array<std::optional<uint8_t>, 31> vics;

    uint8_t valid_vics() const {
      uint8_t result = 0;
      for (const auto& vic : vics)
        if (vic.has_value())
          ++result;
      return result;
    }

    size_t size() const {
      return valid_vics() * sizeof(uint8_t) + CTA861_DATA_BLOCK_HEADER_SIZE;
    }

    CtaDataBlockType type() const {
      return CtaDataBlockType(CTA861_VIDEO_DATA_BLOCK_TAG);
    }

    uint8_t native_vics() const {
      uint8_t result = 0;
      for (const auto& vic : vics)
        if (vic.has_value())
          if (vic.value() >= 129 && vic.value() <= 192)
            ++result;
      return result;
    }

    std::vector<uint8_t> generate_byte_block() const;
    void print(std::ostream& os, uint8_t tabs = 1) const;

    template<typename Iterator>
    static VideoDataBlock parse_byte_block(Iterator start) {
      VideoDataBlock result;
      int pos = 0;

      int data_block_tag = *start >> 5 & BITMASK_TRUE(3);
      if (data_block_tag != CTA861_VIDEO_DATA_BLOCK_TAG)
        throw EdidException(__FUNCTION__, "Video Data Block has incorrect Data Block Tag: " +
          std::to_string(data_block_tag)
        );

      int vics = *start & BITMASK_TRUE(5);

      for (int i = 0; i < vics; ++i) {
        result.vics[i] = *(start + ++pos);
      }

      return result;
    }
  };

  bool operator==(const VideoDataBlock& lhs, const VideoDataBlock& rhs);

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

  bool operator==(const ShortAudioDescriptor& lhs, const ShortAudioDescriptor& rhs);

  struct AudioDataBlock {
    std::array<std::optional<ShortAudioDescriptor>, 10> sads;

    uint8_t valid_sads() const {
      uint8_t result = 0;
      for (const auto& sad : sads)
        if (sad.has_value())
          ++result;
      return result;
    }

    size_t size() const {
      return valid_sads() * ShortAudioDescriptor::size() + CTA861_DATA_BLOCK_HEADER_SIZE;
    }

    CtaDataBlockType type() const {
      return CtaDataBlockType(CTA861_AUDIO_DATA_BLOCK_TAG);
    }

    std::vector<uint8_t> generate_byte_block() const;
    void print(std::ostream& os, uint8_t tabs = 1) const;

    template<typename Iterator>
    static AudioDataBlock parse_byte_block(Iterator start) {
      AudioDataBlock result;
      int pos = 0;

      int data_block_tag = *start >> 5 & BITMASK_TRUE(3);
      if (data_block_tag != CTA861_AUDIO_DATA_BLOCK_TAG)
        throw EdidException(__FUNCTION__, "Audio Data Block has incorrect header: " +
          std::to_string(data_block_tag));

      int sads = *start & BITMASK_TRUE(5);

      if (sads % ShortAudioDescriptor::size() != 0)
        throw EdidException(__FUNCTION__, "Audio Data Block has size which is not"
          "a factor of " + std::to_string(ShortAudioDescriptor::size()) +
          ": " + std::to_string(sads));
      else
        sads /= ShortAudioDescriptor::size();

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

  bool operator==(const AudioDataBlock& lhs, const AudioDataBlock& rhs);

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

  struct SpeakerAllocationDataBlock {
    SpeakerAllocation speaker_allocation = 0;

    size_t size() const {
      return CTA861_SPEAKERS_DATA_BLOCK_LENGTH + CTA861_DATA_BLOCK_HEADER_SIZE;
    }

    CtaDataBlockType type() const {
      return CtaDataBlockType(CTA861_SPEAKERS_DATA_BLOCK_TAG);
    }

    std::vector<uint8_t> generate_byte_block() const;
    void print(std::ostream& os, uint8_t tabs = 1) const;

    template<typename Iterator>
    static SpeakerAllocationDataBlock parse_byte_block(Iterator start) {
      SpeakerAllocationDataBlock result;
      int pos = 0;

      int data_block_tag = *start >> 5 & BITMASK_TRUE(3);
      if (data_block_tag != CTA861_SPEAKERS_DATA_BLOCK_TAG)
        throw EdidException(__FUNCTION__, "Speaker Allocation Data Block has incorrect header: " +
          std::to_string(data_block_tag));

      int data_block_size = *start & BITMASK_TRUE(5);

      if (data_block_size != CTA861_SPEAKERS_DATA_BLOCK_LENGTH)
        throw EdidException(__FUNCTION__, "Speaker Allocation Data Block has invalid length: " +
          std::to_string(data_block_size));

      result.speaker_allocation = *(start + ++pos);

      return result;
    }
  };

  bool operator==(const SpeakerAllocationDataBlock& lhs, const SpeakerAllocationDataBlock& rhs);

  struct YCbCr420CapabilityMapDataBlock {
    std::set<uint8_t> svd_indices;

    size_t size() const {
      if (!svd_indices.size())
        return CTA861_DATA_BLOCK_HEADER_SIZE + CTA861_EXTENDED_TAG_SIZE;
      auto last_element = svd_indices.rbegin();
      const size_t bytes = *last_element / 8 + (*last_element % 8 == 0 ? 0 : 1);
      return bytes + CTA861_DATA_BLOCK_HEADER_SIZE + CTA861_EXTENDED_TAG_SIZE;
    }

    CtaDataBlockType type() const {
      return CtaDataBlockType(CTA861_EXTENDED_TAG, CTA861_EXTENDED_YCBCR420_CAPABILITY_MAP_DATA_BLOCK_TAG);
    }

    std::vector<uint8_t> generate_byte_block() const;
    void print(std::ostream& os, uint8_t tabs = 1) const;

    template<typename Iterator>
    static YCbCr420CapabilityMapDataBlock parse_byte_block(Iterator start) {
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

  bool operator==(const YCbCr420CapabilityMapDataBlock& lhs, const YCbCr420CapabilityMapDataBlock& rhs);

  bool operator==(const CtaDataBlockType& lhs, const CtaDataBlockType& rhs);

}  // namespace Edid
