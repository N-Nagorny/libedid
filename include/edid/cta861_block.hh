#pragma once

#include <array>
#include <memory>
#include <optional>
#include <vector>

#include "common.hh"

#define CTA861_EXT_TAG 0x02
#define CTA861_VERSION 3

#define CTA861_DATA_BLOCK_HEADER_SIZE 1

#define CTA861_AUDIO_DATA_BLOCK_TAG 0b001
#define CTA861_VIDEO_DATA_BLOCK_TAG 0b010
#define CTA861_VENDOR_DATA_BLOCK_TAG 0b011
#define CTA861_SPEAKERS_DATA_BLOCK_TAG 0b100
#define CTA861_VESA_DISPLAY_TRANSFER_DATA_BLOCK_TAG 0b101

#define CTA861_SPEAKERS_DATA_BLOCK_LENGTH 3

namespace Edid {
  class CtaDataBlock {
   public:
    virtual ~CtaDataBlock() = default;

    virtual uint8_t size() const = 0;
  };

  struct CtaDataBlockWrapper {
    uint8_t data_block_tag;
    uint8_t data_block_length;
    std::shared_ptr<CtaDataBlock> data_block_ptr;
  };

  bool operator==(const CtaDataBlockWrapper& lhs, const CtaDataBlockWrapper& rhs);

  struct UnknownDataBlock : CtaDataBlock {
    UnknownDataBlock(const std::vector<uint8_t>& raw_data)
      : raw_data(raw_data)
    {}
    std::vector<uint8_t> raw_data;
    uint8_t size() const override {
      return raw_data.size() + CTA861_DATA_BLOCK_HEADER_SIZE;
    }
  };

  bool operator==(const UnknownDataBlock& lhs, const UnknownDataBlock& rhs);

  struct VideoDataBlock : CtaDataBlock {
    std::array<std::optional<uint8_t>, 31> vics;

    uint8_t valid_vics() const {
      uint8_t result = 0;
      for (const auto& vic : vics)
        if (vic.has_value())
          ++result;
      return result;
    }

    uint8_t size() const override {
      return valid_vics() * sizeof(uint8_t) + CTA861_DATA_BLOCK_HEADER_SIZE;
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

  struct AudioDataBlock : CtaDataBlock {
    std::array<std::optional<ShortAudioDescriptor>, 10> sads;

    uint8_t valid_sads() const {
      uint8_t result = 0;
      for (const auto& sad : sads)
        if (sad.has_value())
          ++result;
      return result;
    }

    uint8_t size() const override {
      return valid_sads() * ShortAudioDescriptor::size() + CTA861_DATA_BLOCK_HEADER_SIZE;
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

  struct SpeakerAllocationDataBlock : CtaDataBlock {
    SpeakerAllocation speaker_allocation = 0;

    uint8_t size() const override {
      return CTA861_SPEAKERS_DATA_BLOCK_LENGTH + CTA861_DATA_BLOCK_HEADER_SIZE;
    }
  };

  bool operator==(const SpeakerAllocationDataBlock& lhs, const SpeakerAllocationDataBlock& rhs);

  using DataBlockCollection = std::vector<CtaDataBlockWrapper>;

  // See CTA-861-G Section 7.5
  struct Cta861Block {
    bool underscan = false;
    bool basic_audio = false;
    bool ycbcr_444 = false;
    bool ycbcr_422 = false;
    DataBlockCollection data_block_collection;
    std::vector<DetailedTimingDescriptor> detailed_timing_descriptors;
  };

  bool operator==(const Cta861Block& lhs, const Cta861Block& rhs);

  std::vector<uint8_t> generate_video_data_block(const VideoDataBlock& video_data_block);
  std::vector<uint8_t> generate_audio_data_block(const AudioDataBlock& audio_data_block);
  std::vector<uint8_t> generate_speaker_allocation_data_block(const SpeakerAllocationDataBlock& speaker_allocation_data_block);

  std::vector<uint8_t> generate_data_block_collection(const DataBlockCollection& collection);
  std::array<uint8_t, EDID_BLOCK_SIZE> generate_cta861_block(const Cta861Block& cta861_block);

  VideoDataBlock parse_video_data_block(const std::vector<uint8_t>& video_data_block);
  AudioDataBlock parse_audio_data_block(const std::vector<uint8_t>& audio_data_block);
  SpeakerAllocationDataBlock parse_speaker_allocation_data_block(const std::vector<uint8_t>& speaker_allocation_data_block);

  DataBlockCollection parse_data_block_collection(const std::vector<uint8_t>& collection);
  Cta861Block parse_cta861_block(const std::array<uint8_t, EDID_BLOCK_SIZE>& cta861);

  void print_cta861_block(std::ostream& os, const Cta861Block& cta861_block);
}
