// Copyright 2023 N-Nagorny
#include "edid/cta_data_block.hh"

namespace Edid {
  std::vector<uint8_t> UnknownDataBlock::generate_byte_block() const {
    std::vector<uint8_t> result(size(), 0x00);
    int pos = 0;

    result[pos] = (data_block_tag & BITMASK_TRUE(3)) << 5;
    result[pos++] |= (size() - CTA861_DATA_BLOCK_HEADER_SIZE) & BITMASK_TRUE(5);
    if (extended_tag.has_value()) {
      result[pos++] = extended_tag.value();
    }

    std::copy(raw_data.begin(), raw_data.end(), result.begin() + pos);

    return result;
  }

  void UnknownDataBlock::print(std::ostream& os, uint8_t tabs) const {
    std::string indent(tabs, '\t');
    os << indent << "Unrecognized Data Block with tag " <<
      std::hex << unsigned(data_block_tag) <<
      (extended_tag.has_value() ? std::to_string(extended_tag.value()) : std::string{}) <<
      std::dec << '\n';
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

  std::vector<uint8_t> YCbCr420CapabilityMapDataBlock::generate_byte_block() const {
    std::vector<uint8_t> result(size(), 0x00);
    int pos = 0;

    result[pos] = CTA861_EXTENDED_TAG << 5;
    result[pos++] |= (size() - CTA861_DATA_BLOCK_HEADER_SIZE) & BITMASK_TRUE(5);
    result[pos++] = CTA861_EXTENDED_YCBCR420_CAPABILITY_MAP_DATA_BLOCK_TAG;

    for (uint8_t svd_index : svd_indices) {
      uint8_t byte_index = (svd_index - 1) / 8;
      uint8_t bit_index = (svd_index - 1) % 8;
      result[pos + byte_index] |= BITMASK_TRUE(1) << bit_index;
    }

    return result;
  }

  void YCbCr420CapabilityMapDataBlock::print(std::ostream& os, uint8_t tabs) const {
    os << "YCbCr420CapabilityMapDataBlock: ";
    for (uint8_t svd_index : svd_indices)
      os << (int)svd_index << " ";
    os << '\n';
  }

  std::vector<uint8_t> ColorimetryDataBlock::generate_byte_block() const {
    std::vector<uint8_t> result(size(), 0x00);
    int pos = 0;

    result[pos] = CTA861_EXTENDED_TAG << 5;
    result[pos++] |= (size() - CTA861_DATA_BLOCK_HEADER_SIZE) & BITMASK_TRUE(5);
    result[pos++] = CTA861_EXTENDED_COLORIMETRY_BLOCK_TAG;

    result[pos++] = colorimetry_standards & BITMASK_TRUE(8);
    result[pos] = (colorimetry_standards >> 12 & BITMASK_TRUE(4)) << 4;
    result[pos] |= gamut_metadata_profiles & BITMASK_TRUE(4);

    return result;
  }

  void ColorimetryDataBlock::print(std::ostream& os, uint8_t tabs) const {
    std::string indent(tabs, '\t');
    // const auto color_standards = split_colorimetry_standards();

    os << indent << "Colorimetry Data Block: \n";
    indent += '\t';

    for (const auto& standard : bitfield_to_enums<ColorimetryStandard>(colorimetry_standards)) {
      os << indent << to_string(standard) << '\n';
    }
    for (const auto& profile : bitfield_to_enums<GamutMetadataProfile>(gamut_metadata_profiles)) {
      os << indent << to_string(profile) << '\n';
    }
  }
}  // namespace Edid
