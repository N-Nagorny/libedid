#include "edid/cta_data_block.hh"

namespace Edid {
  std::vector<uint8_t> UnknownDataBlock::generate_byte_block() const {
    std::vector<uint8_t> result(size(), 0x00);
    int pos = 0;

    result[pos] = (data_block_tag & BITMASK_TRUE(3)) << 5;
    result[pos++] |= (size() - CTA861_DATA_BLOCK_HEADER_SIZE) & BITMASK_TRUE(5);

    std::copy(raw_data.begin(), raw_data.end(), result.begin() + pos);

    return result;
  }

  void UnknownDataBlock::print(std::ostream& os, uint8_t tabs) const {
    os << "Unrecognized Data Block with tag " <<
      std::hex << unsigned(data_block_tag) <<
      std::dec << '\n';
  }

  bool operator==(const UnknownDataBlock& lhs, const UnknownDataBlock& rhs) {
    return std::tie(lhs.raw_data, lhs.data_block_tag) ==
      std::tie(rhs.raw_data, rhs.data_block_tag);
  }

  std::vector<uint8_t> VideoDataBlock::generate_byte_block() const {
    std::vector<uint8_t> result(size(), 0x0);
    int pos = 0;

    result[pos] = CTA861_VIDEO_DATA_BLOCK_TAG << 5;
    result[pos++] |= valid_vics() & BITMASK_TRUE(5);

    for (const std::optional<uint8_t>& vic : vics)
      if (vic.has_value())
      result[pos++] = vic.value();

    return result;
  }

  void VideoDataBlock::print(std::ostream& os, uint8_t tabs) const {
    os << "Video Data Block: ";
    for (const std::optional<uint8_t>& vic : vics)
      if (vic.has_value())
      os << (int)vic.value() << " ";
    os << '\n';
  }

  bool operator==(const VideoDataBlock& lhs, const VideoDataBlock& rhs) {
    return lhs.vics == rhs.vics;
  }

  std::vector<uint8_t> AudioDataBlock::generate_byte_block() const {
    std::vector<uint8_t> result(size(), 0x0);
    int pos = 0;

    result[pos] = CTA861_AUDIO_DATA_BLOCK_TAG << 5;
    result[pos++] |= valid_sads() * ShortAudioDescriptor::size() & BITMASK_TRUE(5);

    for (const std::optional<ShortAudioDescriptor>& sad : sads) {
      if (sad.has_value()) {
        result[pos] = sad->audio_format << 3;
        result[pos++] |= sad->channels;
        result[pos++] = sad->sampling_freqs;
        result[pos++] = sad->lpcm_bit_depths;
      }
    }

    return result;
  }

  void AudioDataBlock::print(std::ostream& os, uint8_t tabs) const {
    os << "Audio Data Block:\n";
    for (const std::optional<ShortAudioDescriptor>& sad : sads) {
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

  std::vector<uint8_t> SpeakerAllocationDataBlock::generate_byte_block() const {
    std::vector<uint8_t> result(size(), 0x00);
    int pos = 0;

    result[pos] = CTA861_SPEAKERS_DATA_BLOCK_TAG << 5;
    result[pos++] |= (size() - CTA861_DATA_BLOCK_HEADER_SIZE) & BITMASK_TRUE(5);

    result[pos++] = speaker_allocation;
    result[pos++] = 0;
    result[pos] = 0;

    return result;
  }

  void SpeakerAllocationDataBlock::print(std::ostream& os, uint8_t tabs) const {
    os << "Speaker Allocation Data Block:\n";
    for (Speaker speaker : bitfield_to_enums<Speaker>(speaker_allocation))
      os << '\t' << to_string(speaker) << '\n';
    os << '\n';
  }
}