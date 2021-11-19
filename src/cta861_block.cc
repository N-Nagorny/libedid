#include <array>
#include <cstdint>
#include <iostream>

#include "common.hh"
#include "cta861_block.hh"
#include "exceptions.hh"

namespace Edid {
  bool operator==(const VideoDataBlock& lhs, const VideoDataBlock& rhs) {
    return lhs.vics == rhs.vics;
  }

  bool operator==(const ShortAudioDescriptor& lhs, const ShortAudioDescriptor& rhs) {
    return std::tie(lhs.audio_format, lhs.channels, lhs.sampling_freqs, lhs.lpcm_bit_depths) ==
      std::tie(rhs.audio_format, rhs.channels, rhs.sampling_freqs, rhs.lpcm_bit_depths);
  }

  bool operator==(const AudioDataBlock& lhs, const AudioDataBlock& rhs) {
    return lhs.sads == rhs.sads;
  }

  bool operator==(const SpeakerAllocationDataBlock& lhs, const SpeakerAllocationDataBlock& rhs) {
    return lhs.speaker_allocation == rhs.speaker_allocation;
  }

  bool operator==(const Cta861Block& lhs, const Cta861Block& rhs) {
    return std::tie(lhs.underscan, lhs.basic_audio, lhs.ycbcr_444, lhs.ycbcr_422, lhs.data_block_collection, lhs.detailed_timing_descriptors) ==
      std::tie(rhs.underscan, rhs.basic_audio, rhs.ycbcr_444, rhs.ycbcr_422, rhs.data_block_collection, rhs.detailed_timing_descriptors);
  }

  bool operator==(const DataBlockCollection& lhs, const DataBlockCollection& rhs) {
    return std::tie(lhs.video_data_block, lhs.audio_data_block, lhs.speaker_allocation_data_block) ==
      std::tie(rhs.video_data_block, rhs.audio_data_block, rhs.speaker_allocation_data_block);
  }

  std::vector<uint8_t> generate_video_data_block(const VideoDataBlock& video_data_block) {
    std::vector<uint8_t> result(video_data_block.size(), 0x0);
    int pos = 0;

    result[pos] = CTA861_VIDEO_DATA_BLOCK_TAG << 5;
    result[pos++] |= video_data_block.valid_vics() & BITMASK_TRUE(5);

    for (const std::optional<uint8_t>& vic : video_data_block.vics)
      if (vic.has_value())
        result[pos++] = vic.value();

    return result;
  }

  std::vector<uint8_t> generate_audio_data_block(const AudioDataBlock& audio_data_block) {
    std::vector<uint8_t> result(audio_data_block.size(), 0x0);
    int pos = 0;

    result[pos] = CTA861_AUDIO_DATA_BLOCK_TAG << 5;
    result[pos++] |= audio_data_block.valid_sads() * ShortAudioDescriptor::size() & BITMASK_TRUE(5);

    for (const std::optional<ShortAudioDescriptor>& sad : audio_data_block.sads) {
      if (sad.has_value()) {
        result[pos] = sad->audio_format << 3;
        result[pos++] |= sad->channels;
        result[pos++] = sad->sampling_freqs;
        result[pos++] = sad->lpcm_bit_depths;
      }
    }

    return result;
  }

  std::array<uint8_t, SpeakerAllocationDataBlock::size()> generate_speaker_allocation_data_block(const SpeakerAllocationDataBlock& speaker_allocation_data_block) {
    std::array<uint8_t, SpeakerAllocationDataBlock::size()> result;
    result.fill(0x0);
    int pos = 0;

    result[pos] = CTA861_SPEAKERS_DATA_BLOCK_TAG << 5;
    result[pos++] |= SpeakerAllocationDataBlock::size() & BITMASK_TRUE(5);

    result[pos++] = 0;
    result[pos++] = speaker_allocation_data_block.speaker_allocation;
    result[pos++] = 0;
    result[pos++] = 0;

    return result;
  }

  std::vector<uint8_t> generate_data_block_collection(const DataBlockCollection& collection) {
    std::vector<uint8_t> result(collection.size(), 0x0);

    std::vector<uint8_t> video_data_block = generate_video_data_block(collection.video_data_block);
    std::vector<uint8_t> audio_data_block = generate_audio_data_block(collection.audio_data_block);
    std::array<uint8_t, SpeakerAllocationDataBlock::size()> speaker_allocation_data_block =
      generate_speaker_allocation_data_block(collection.speaker_allocation_data_block);

    auto pos = std::move(video_data_block.begin(), video_data_block.end(), result.begin());
    pos = std::move(audio_data_block.begin(), audio_data_block.end(), pos);
    std::move(speaker_allocation_data_block.begin(), speaker_allocation_data_block.end(), pos);

    return result;
  }

  std::array<uint8_t, EDID_BLOCK_SIZE> generate_cta861_block(const Cta861Block& cta861) {
    const int header_size = 4;
    const int checksum_size = 1;
    const int dtd_block_size = cta861.detailed_timing_descriptors.size() * DTD_BLOCK_SIZE;
    const int data_block_collection_size = cta861.data_block_collection.size();

    if (header_size + data_block_collection_size + dtd_block_size + checksum_size > EDID_BLOCK_SIZE)
      throw EdidException(__FUNCTION__,
        "CTA861 header (" + std::to_string(header_size) + " B) +"
        " data block collection (" + std::to_string(data_block_collection_size) + " B) +"
        " DTD block (" + std::to_string(dtd_block_size) + " B) +"
        " checksum (" + std::to_string(checksum_size) + " B)"
        " take more than " + std::to_string(EDID_BLOCK_SIZE) + " B.");

    std::array<uint8_t, EDID_BLOCK_SIZE> result;
    result.fill(0x0);
    int pos = 0;

    // Header
    result[pos++] = CTA861_EXT_TAG;
    result[pos++] = CTA861_VERSION;

    int dtd_start_pos = 0x0;
    if (!cta861.detailed_timing_descriptors.empty())
      dtd_start_pos = 0x4 + data_block_collection_size;

    result[pos++] = dtd_start_pos;

    // No Native DTDs
    result[pos] = cta861.underscan << 7;
    result[pos] |= cta861.basic_audio << 6;
    result[pos] |= cta861.ycbcr_444 << 5;
    result[pos++] |= cta861.ycbcr_422 << 4;

    std::vector<uint8_t> data_block_collection = generate_data_block_collection(cta861.data_block_collection);
    std::move(data_block_collection.begin(), data_block_collection.end(), result.begin() + pos);
    pos += data_block_collection_size;

    for (const auto& detailed_timing_descriptor : cta861.detailed_timing_descriptors) {
      auto dtd = make_dtd(detailed_timing_descriptor);
      std::move(dtd.begin(), dtd.end(), result.begin() + pos);
      pos += dtd.size();
    }

    while (pos < EDID_BLOCK_SIZE - 1)
      result[pos++] = 0x0;

    result[pos] = calculate_block_checksum(result);

    return result;
  }

  VideoDataBlock parse_video_data_block(const std::vector<uint8_t>& video_data_block) {
    VideoDataBlock result;
    int pos = 0;

    int data_block_tag = video_data_block[pos] >> 5 & BITMASK_TRUE(3);
    if (data_block_tag != CTA861_VIDEO_DATA_BLOCK_TAG)
      throw EdidException(__FUNCTION__, "Video Data Block has incorrect header: " +
        std::to_string(data_block_tag));

    int vics = video_data_block[pos] & BITMASK_TRUE(5);

    for (int i = 0; i < vics; ++i) {
      result.vics[i] = video_data_block[++pos];
    }

    return result;
  }

  AudioDataBlock parse_audio_data_block(const std::vector<uint8_t>& audio_data_block) {
    AudioDataBlock result;
    int pos = 0;

    int data_block_tag = audio_data_block[pos] >> 5 & BITMASK_TRUE(3);
    if (data_block_tag != CTA861_AUDIO_DATA_BLOCK_TAG)
      throw EdidException(__FUNCTION__, "Audio Data Block has incorrect header: " +
        std::to_string(data_block_tag));

    int sads = audio_data_block[pos] & BITMASK_TRUE(5);

    if (sads % ShortAudioDescriptor::size() != 0)
      throw EdidException(__FUNCTION__, "Audio Data Block has size which is not"
        "a factor of " + std::to_string(ShortAudioDescriptor::size()) +
        ": " + std::to_string(sads));
    else
      sads /= ShortAudioDescriptor::size();

    for (int i = 0; i < sads; ++i) {
      ShortAudioDescriptor sad;
      sad.audio_format = AudioFormatCode(audio_data_block[++pos] >> 3 & BITMASK_TRUE(4));
      sad.channels = AudioChannels(audio_data_block[pos] & BITMASK_TRUE(3));
      sad.sampling_freqs = audio_data_block[++pos];
      sad.lpcm_bit_depths = audio_data_block[++pos];
      result.sads[i] = sad;
    }

    return result;
  }

  SpeakerAllocationDataBlock parse_speaker_allocation_data_block(const std::array<uint8_t, SpeakerAllocationDataBlock::size()>& speaker_allocation_data_block) {
    SpeakerAllocationDataBlock result;
    int pos = 0;

    int data_block_tag = speaker_allocation_data_block[pos] >> 5 & BITMASK_TRUE(3);
    if (data_block_tag != CTA861_SPEAKERS_DATA_BLOCK_TAG)
      throw EdidException(__FUNCTION__, "Speaker Allocation Data Block has incorrect header: " +
        std::to_string(data_block_tag));

    int data_block_size = speaker_allocation_data_block[pos] & BITMASK_TRUE(5);

    if (data_block_size != SpeakerAllocationDataBlock::size())
      throw EdidException(__FUNCTION__, "Speaker Allocation Data Block has invalid size: " +
        std::to_string(data_block_size));

    pos += 2;
    result.speaker_allocation = speaker_allocation_data_block[pos];

    return result;
  }

  DataBlockCollection parse_data_block_collection(const std::vector<uint8_t>& collection) {
    DataBlockCollection result;

    result.video_data_block = parse_video_data_block(collection);
    result.audio_data_block = parse_audio_data_block(std::vector<uint8_t>(
      collection.begin() + result.video_data_block.size(),
      collection.end()
    ));
    std::array<uint8_t, SpeakerAllocationDataBlock::size()> speaker_allocation_data_block;
    std::move(
      collection.begin() + result.video_data_block.size() + result.audio_data_block.size(),
      collection.end(),
      speaker_allocation_data_block.begin()
    );
    result.speaker_allocation_data_block = parse_speaker_allocation_data_block(speaker_allocation_data_block);

    return result;
  }

  Cta861Block parse_cta861_block(const std::array<uint8_t, EDID_BLOCK_SIZE>& cta861) {
    Cta861Block result;
    int pos = 0;

    // Header
    if (cta861[pos] != CTA861_EXT_TAG)
      throw EdidException(__FUNCTION__, "CTA Extension has invalid Extension Tag: " +
        std::to_string(cta861[pos]));
    pos++;
    if (cta861[pos] != CTA861_VERSION)
      throw EdidException(__FUNCTION__, "CTA Extension has unsupported version: " +
        std::to_string(cta861[pos]));

    // Checksum
    if (cta861[EDID_BLOCK_SIZE - 1] != calculate_block_checksum(cta861))
      throw EdidException(__FUNCTION__, "CTA Extension checksum is invalid.");

    int dtd_start_pos = cta861[++pos];

    result.underscan = cta861[++pos] >> 7 & BITMASK_TRUE(1);
    result.basic_audio = cta861[pos] >> 6 & BITMASK_TRUE(1);
    result.ycbcr_444 = cta861[pos] >> 5 & BITMASK_TRUE(1);
    result.ycbcr_422 = cta861[pos++] >> 4 & BITMASK_TRUE(1);

    if (dtd_start_pos != 0) {
      if (dtd_start_pos != pos) {
        result.data_block_collection = parse_data_block_collection(std::vector<uint8_t>(
          cta861.begin() + pos,
          cta861.begin() + dtd_start_pos
        ));
        pos += result.data_block_collection.size();
      }

      while (cta861[pos] != 0 && cta861[pos + 1] != 0) {
        std::array<uint8_t, DTD_BLOCK_SIZE> dtd_binary;
        std::move(
          cta861.begin() + pos,
          cta861.begin() + pos + DTD_BLOCK_SIZE,
          dtd_binary.begin()
        );
        result.detailed_timing_descriptors.push_back(parse_dtd(dtd_binary));
        pos += DTD_BLOCK_SIZE;
      }
    }

    return result;
  }
}
