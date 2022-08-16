#include <array>
#include <cstdint>
#include <iostream>
#include <numeric>

#include "edid/common.hh"
#include "edid/cta861_block.hh"
#include "edid/exceptions.hh"

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

  bool operator==(const CtaDataBlockWrapper& lhs, const CtaDataBlockWrapper& rhs) {
    bool headers_equal = std::tie(lhs.data_block_tag, lhs.data_block_length) ==
      std::tie(rhs.data_block_tag, rhs.data_block_length);
    if (!headers_equal)
      return false;

    bool content_equal = false;
    switch (lhs.data_block_tag) {
      case CTA861_VIDEO_DATA_BLOCK_TAG:
        content_equal = *std::dynamic_pointer_cast<VideoDataBlock>(lhs.data_block_ptr) ==
          *std::dynamic_pointer_cast<VideoDataBlock>(rhs.data_block_ptr);
        break;
      case CTA861_AUDIO_DATA_BLOCK_TAG:
        content_equal = *std::dynamic_pointer_cast<AudioDataBlock>(lhs.data_block_ptr) ==
          *std::dynamic_pointer_cast<AudioDataBlock>(rhs.data_block_ptr);
        break;
      case CTA861_SPEAKERS_DATA_BLOCK_TAG:
        content_equal = *std::dynamic_pointer_cast<SpeakerAllocationDataBlock>(lhs.data_block_ptr) ==
          *std::dynamic_pointer_cast<SpeakerAllocationDataBlock>(rhs.data_block_ptr);
        break;
      default:
        content_equal = *std::dynamic_pointer_cast<UnknownDataBlock>(lhs.data_block_ptr) ==
          *std::dynamic_pointer_cast<UnknownDataBlock>(rhs.data_block_ptr);
    }
    return content_equal;
  }

  bool operator==(const UnknownDataBlock& lhs, const UnknownDataBlock& rhs) {
    return lhs.raw_data == rhs.raw_data;
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

  std::vector<uint8_t> generate_speaker_allocation_data_block(const SpeakerAllocationDataBlock& speaker_allocation_data_block) {
    std::vector<uint8_t> result(speaker_allocation_data_block.size(), 0x00);
    int pos = 0;

    result[pos] = CTA861_SPEAKERS_DATA_BLOCK_TAG << 5;
    result[pos++] |= (speaker_allocation_data_block.size() - CTA861_DATA_BLOCK_HEADER_SIZE) & BITMASK_TRUE(5);

    result[pos++] = speaker_allocation_data_block.speaker_allocation;
    result[pos++] = 0;
    result[pos] = 0;

    return result;
  }

  std::vector<uint8_t> generate_unknown_data_block(uint8_t tag, const UnknownDataBlock& unknown_data_block) {
    std::vector<uint8_t> result(unknown_data_block.size(), 0x00);
    int pos = 0;

    result[pos] = (tag & BITMASK_TRUE(3)) << 5;
    result[pos++] |= (unknown_data_block.size() - CTA861_DATA_BLOCK_HEADER_SIZE) & BITMASK_TRUE(5);

    std::copy(unknown_data_block.raw_data.begin(), unknown_data_block.raw_data.end(), result.begin() + pos);

    return result;
  }

  std::vector<uint8_t> generate_data_block_collection(const DataBlockCollection& collection) {
    size_t collection_size = std::accumulate(collection.begin(), collection.end(), 0, [](size_t size, const CtaDataBlockWrapper& wrapper) {
      return size + wrapper.data_block_length;
    });
    std::vector<uint8_t> result(collection_size, 0x0);
    auto pos = result.begin();

    for (const auto& wrapper : collection) {
      std::vector<uint8_t> data_block;
      switch (wrapper.data_block_tag) {
        case CTA861_VIDEO_DATA_BLOCK_TAG:
          data_block = generate_video_data_block(*std::dynamic_pointer_cast<VideoDataBlock>(wrapper.data_block_ptr));
          break;
        case CTA861_AUDIO_DATA_BLOCK_TAG:
          data_block = generate_audio_data_block(*std::dynamic_pointer_cast<AudioDataBlock>(wrapper.data_block_ptr));
          break;
        case CTA861_SPEAKERS_DATA_BLOCK_TAG:
          data_block = generate_speaker_allocation_data_block(*std::dynamic_pointer_cast<SpeakerAllocationDataBlock>(wrapper.data_block_ptr));
          break;
        default:
          data_block = generate_unknown_data_block(wrapper.data_block_tag, *std::dynamic_pointer_cast<UnknownDataBlock>(wrapper.data_block_ptr));
      }
      pos = std::move(data_block.begin(), data_block.end(), pos);
    }

    return result;
  }

  std::array<uint8_t, EDID_BLOCK_SIZE> generate_cta861_block(const Cta861Block& cta861) {
    const int header_size = 4;
    const int checksum_size = 1;
    const int dtd_block_size = cta861.detailed_timing_descriptors.size() * DTD_BLOCK_SIZE;
    const int data_block_collection_size = std::accumulate(cta861.data_block_collection.begin(), cta861.data_block_collection.end(), 0, [](size_t size, const CtaDataBlockWrapper& wrapper) {
      return size + wrapper.data_block_length;
    });

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
      throw EdidException(__FUNCTION__, "Video Data Block has incorrect Data Block Tag: " +
        std::to_string(data_block_tag));

    int vics = video_data_block[pos] & BITMASK_TRUE(5);

    for (int i = 0; i < vics; ++i) {
      uint8_t byte = video_data_block[++pos];
      // Exclude Native Timing indicator bit from VIC
      if (byte >= 128 && byte <= 192)
        byte -= 128;
      result.vics[i] = byte;
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

  SpeakerAllocationDataBlock parse_speaker_allocation_data_block(const std::vector<uint8_t>& speaker_allocation_data_block) {
    SpeakerAllocationDataBlock result;
    int pos = 0;

    int data_block_tag = speaker_allocation_data_block[pos] >> 5 & BITMASK_TRUE(3);
    if (data_block_tag != CTA861_SPEAKERS_DATA_BLOCK_TAG)
      throw EdidException(__FUNCTION__, "Speaker Allocation Data Block has incorrect header: " +
        std::to_string(data_block_tag));

    int data_block_size = speaker_allocation_data_block[pos] & BITMASK_TRUE(5);

    if (data_block_size != CTA861_SPEAKERS_DATA_BLOCK_LENGTH)
      throw EdidException(__FUNCTION__, "Speaker Allocation Data Block has invalid length: " +
        std::to_string(data_block_size));

    result.speaker_allocation = speaker_allocation_data_block[++pos];

    return result;
  }

  UnknownDataBlock parse_unknown_data_block(const std::vector<uint8_t>& unknown_data_block) {
    int pos = 0;

    int data_block_tag = unknown_data_block[pos] >> 5 & BITMASK_TRUE(3);
    int data_block_size = unknown_data_block[pos++] & BITMASK_TRUE(5);
    return UnknownDataBlock(
      std::vector<uint8_t>(unknown_data_block.begin() + pos, unknown_data_block.begin() + pos + data_block_size)
    );
  }
  DataBlockCollection parse_data_block_collection(const std::vector<uint8_t>& collection) {
    DataBlockCollection result;
    auto iter_read = collection.begin();
    std::shared_ptr<CtaDataBlock> data_block_ptr = nullptr;

    while (iter_read < collection.end()) {
      uint8_t data_block_tag = *iter_read >> 5 & BITMASK_TRUE(3);
      switch (data_block_tag) {
        case CTA861_VIDEO_DATA_BLOCK_TAG:
          data_block_ptr = std::make_unique<VideoDataBlock>(std::move(parse_video_data_block(std::vector<uint8_t>(
            iter_read,
            collection.end()
          ))));
          break;
        case CTA861_AUDIO_DATA_BLOCK_TAG:
          data_block_ptr = std::make_unique<AudioDataBlock>(std::move(parse_audio_data_block(std::vector<uint8_t>(
            iter_read,
            collection.end()
          ))));
          break;
        case CTA861_SPEAKERS_DATA_BLOCK_TAG:
          data_block_ptr = std::make_unique<SpeakerAllocationDataBlock>(std::move(parse_speaker_allocation_data_block(std::vector<uint8_t>(
            iter_read,
            collection.end()
          ))));
          break;
        default:
          data_block_ptr = std::make_unique<UnknownDataBlock>(std::move(parse_unknown_data_block(std::vector<uint8_t>(
            iter_read,
            collection.end()
          ))));
      }
      result.push_back({data_block_tag, data_block_ptr->size(), data_block_ptr});
      iter_read += data_block_ptr->size();
    }

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
        pos = dtd_start_pos;
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

  void print_video_data_block(std::ostream& os, const VideoDataBlock& video_data_block) {
    os << "Video Data Block: ";
    for (const std::optional<uint8_t>& vic : video_data_block.vics)
      if (vic.has_value())
        os << (int)vic.value() << " ";
    os << '\n';
  }

  void print_audio_data_block(std::ostream& os, const AudioDataBlock& audio_data_block) {
    os << "Audio Data Block:\n";
    for (const std::optional<ShortAudioDescriptor>& sad : audio_data_block.sads) {
      if (sad.has_value()) {
        os << '\t' << "Short Audio Descriptor:\n";
        os << "\t\t" << to_string(sad->audio_format) << '\n';
        os << "\t\t" << "Up to " << to_string(sad->channels) << " channels\n";
        for (SamplingFrequence sf : bitfield_to_enums<SamplingFrequence>(sad->sampling_freqs))
          os << "\t\t" << to_string(sf) << '\n';
        if (sad->audio_format == AudioFormatCode::LPCM)
          for (LpcmBitDepth bit_depth : bitfield_to_enums<LpcmBitDepth>(sad->lpcm_bit_depths))
            os << "\t\t" << to_string(bit_depth) << '\n';
      }
    }
    os << '\n';
  }

  void print_speaker_allocation_data_block(std::ostream& os, const SpeakerAllocationDataBlock& speaker_allocation_data_block) {
    os << "Speaker Allocation Data Block:\n";
    for (Speaker speaker : bitfield_to_enums<Speaker>(speaker_allocation_data_block.speaker_allocation))
      os << '\t' << to_string(speaker) << '\n';
    os << '\n';
  }

  void print_data_block_collection(std::ostream& os, const DataBlockCollection& collection) {
    for (const auto& wrapper : collection) {
      switch (wrapper.data_block_tag) {
        case CTA861_VIDEO_DATA_BLOCK_TAG:
          print_video_data_block(os, *std::dynamic_pointer_cast<VideoDataBlock>(wrapper.data_block_ptr));
          break;
        case CTA861_AUDIO_DATA_BLOCK_TAG:
          print_audio_data_block(os, *std::dynamic_pointer_cast<AudioDataBlock>(wrapper.data_block_ptr));
          break;
        case CTA861_SPEAKERS_DATA_BLOCK_TAG:
          print_speaker_allocation_data_block(os, *std::dynamic_pointer_cast<SpeakerAllocationDataBlock>(wrapper.data_block_ptr));
          break;
        default:
          os << "Unrecognized Data Block with tag " <<
            std::hex << unsigned(wrapper.data_block_tag) <<
            std::dec << '\n';
      }
    }
  }

  void print_cta861_block(std::ostream& os, const Cta861Block& cta861_block) {
    if (cta861_block.underscan)
      os << "The display underscans IT timings by default" << '\n';
    if (cta861_block.basic_audio)
      os << "The display supports Basic Audio" << '\n';
    if (cta861_block.ycbcr_444)
      os << "The display supports YCbCr 4:4:4" << '\n';
    if (cta861_block.ycbcr_422)
      os << "The display supports YCbCr 4:2:2" << '\n';
    print_data_block_collection(os, cta861_block.data_block_collection);
    os << "Detailed Timing Descriptors:\n";
    for (const auto& detailed_timing_descriptor : cta861_block.detailed_timing_descriptors) {
      print_detailed_timing_descriptor(os, detailed_timing_descriptor);
    }
  }
}
