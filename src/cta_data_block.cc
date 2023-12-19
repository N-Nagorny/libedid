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
    // Note that data blocks with CTA Tag Codes of 1 through 6
    // are limited to containing 31 payload bytes
    //
    // See CTA-861-I Section 7.4
    if (vics.size() > 31)
      throw EdidException("Video Data Block is longer than 31 VICs: " +
        vics.size()
      );

    std::vector<uint8_t> result(size(), 0x0);
    int pos = 0;

    result[pos] = CTA861_VIDEO_DATA_BLOCK_TAG << 5;
    result[pos++] |= vics.size() & BITMASK_TRUE(5);

    std::copy(vics.data(), vics.data() + vics.size(), result.data() + pos);

    return result;
  }

  void VideoDataBlock::print(std::ostream& os, uint8_t tabs) const {
    std::string indent(tabs, '\t');

    os << indent << "Video Data Block:\n";
    indent += '\t';

    for (uint8_t vic : vics)
      os << indent << static_cast<int>(vic) << "\n";
  }

  std::vector<uint8_t> AudioDataBlock::generate_byte_block() const {
    // Each Short Audio Descriptor is 3-bytes long. There can be up to 31 bytes of payload in a Data
    // Block, therefore there may be up to 10 Short Audio Descriptors
    //
    // See CTA-861-I Section 7.5.2
    if (sads.size() > 10)
      throw EdidException("Audio Data Block is longer than 10 SADs: " +
        sads.size()
      );

    std::vector<uint8_t> result(size(), 0x0);
    int pos = 0;

    result[pos] = CTA861_AUDIO_DATA_BLOCK_TAG << 5;
    result[pos++] |= sads.size() * ShortAudioDescriptor::size() & BITMASK_TRUE(5);

    for (const ShortAudioDescriptor& sad : sads) {
      result[pos] = sad.audio_format << 3;
      result[pos++] |= sad.channels;
      result[pos++] = sad.sampling_freqs;
      result[pos++] = sad.lpcm_bit_depths;
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
    os << indent << "Colorimetry Data Block: \n";
    indent += '\t';

    for (const auto& standard : bitfield_to_enums<ColorimetryStandard>(colorimetry_standards)) {
      os << indent << to_string(standard) << '\n';
    }
    for (const auto& profile : bitfield_to_enums<GamutMetadataProfile>(gamut_metadata_profiles)) {
      os << indent << to_string(profile) << '\n';
    }
  }

  HdrStaticMetadataDataBlock HdrStaticMetadataDataBlock::parse_byte_block(const uint8_t* iter) {
    HdrStaticMetadataDataBlock result;

    const uint8_t data_block_tag = *iter >> 5 & BITMASK_TRUE(3);
    uint8_t size_remainder = *iter++ & BITMASK_TRUE(5);
    if (data_block_tag != CTA861_EXTENDED_TAG)
      throw EdidException(__FUNCTION__, "Extended Tag Data Block has incorrect Data Block Tag: " +
        std::to_string(data_block_tag)
      );

    int extended_tag = *iter++; size_remainder--;
    if (extended_tag != CTA861_EXTENDED_HDR_STATIC_METADATA_BLOCK_TAG)
      throw EdidException(__FUNCTION__, "HDR Static Metadata Data Block has incorrect Extended Data Block Tag: " +
        std::to_string(extended_tag)
      );

    result.transfer_functions = *iter++; size_remainder--;
    result.static_metadata_types = *iter++; size_remainder--;

    if (size_remainder > 0) {
      result.max_luminance_code_value = *iter++; size_remainder--;
    }
    if (size_remainder > 0) {
      result.max_frame_average_luminance_code_value = *iter++; size_remainder--;
    }
    if (size_remainder > 0) {
      result.min_luminance_code_value = *iter++;
    }

    return result;
  }

  std::vector<uint8_t> HdrStaticMetadataDataBlock::generate_byte_block() const {
    std::vector<uint8_t> result(size(), 0x00);
    int pos = 0;

    result[pos] = CTA861_EXTENDED_TAG << 5;
    result[pos++] |= (size() - CTA861_DATA_BLOCK_HEADER_SIZE) & BITMASK_TRUE(5);
    result[pos++] = CTA861_EXTENDED_HDR_STATIC_METADATA_BLOCK_TAG;

    result[pos++] = transfer_functions & BITMASK_TRUE(6);
    result[pos++] = static_metadata_types;

    if (max_luminance_code_value.has_value()) {
      result[pos] = *max_luminance_code_value;
    }
    pos++;

    if (max_frame_average_luminance_code_value.has_value()) {
      result[pos] = *max_frame_average_luminance_code_value;
    }
    pos++;

    if (min_luminance_code_value.has_value()) {
      result[pos] = *min_luminance_code_value;
    }

    return result;
  }

  void HdrStaticMetadataDataBlock::print(std::ostream& os, uint8_t tabs) const {
    const auto get_luminance = [](uint8_t cv) -> float {
      return 50.0 * std::pow(2.0, cv / 32.0);
    };

    const auto get_min_luminance = [](uint8_t cv, float max_luminanace) -> float {
      return max_luminanace * std::pow(cv / 255.0, 2.0) / 100.0;
    };

    std::string indent(tabs, '\t');
    os << indent << "HDR Static Metadata Data Block: \n";
    indent += '\t';

    os << indent << "Supported EOTFs:\n";
    for (const auto& tf : bitfield_to_enums<ElectroOpticalTransferFunction>(transfer_functions)) {
      os << indent << '\t' << to_string(tf) << '\n';
    }

    os << indent << "Supported HDR Static Metadata Types:\n";
    for (const auto& type : bitfield_to_enums<StaticMetadataType>(static_metadata_types)) {
      os << indent << '\t' << to_string(type) << '\n';
    }

    if (max_luminance_code_value.has_value()) {
      os << indent << "Desired Max luminance: " <<
        static_cast<uint16_t>(*max_luminance_code_value) << " (" <<
        get_luminance(*max_luminance_code_value) << " cd/m^2)\n";
    }
    if (max_frame_average_luminance_code_value.has_value()) {
      os << indent << "Desired Max frame-average luminance: " <<
        static_cast<uint16_t>(*max_frame_average_luminance_code_value) << " (" <<
        get_luminance(*max_frame_average_luminance_code_value) << " cd/m^2)\n";
    }
    if (min_luminance_code_value.has_value()) {
      os << indent << "Desired Min luminance: " <<
        static_cast<uint16_t>(*min_luminance_code_value) << " (" <<
        get_min_luminance(*min_luminance_code_value, get_luminance(*max_luminance_code_value)) << " cd/m^2)\n";
    }
  }

  VideoCapabilityDataBlock VideoCapabilityDataBlock::parse_byte_block(const uint8_t* iter) {
    VideoCapabilityDataBlock result;

    const uint8_t data_block_tag = *iter++ >> 5 & BITMASK_TRUE(3);
    if (data_block_tag != CTA861_EXTENDED_TAG)
      throw EdidException(__FUNCTION__, "Extended Tag Data Block has incorrect Data Block Tag: " +
        std::to_string(data_block_tag)
      );

    int extended_tag = *iter++;
    if (extended_tag != CTA861_EXTENDED_VIDEO_CAPABILITY_BLOCK_TAG)
      throw EdidException(__FUNCTION__, "Video Capability Data Block has incorrect Extended Data Block Tag: " +
        std::to_string(extended_tag)
      );

    result.ce_scan_behaviour = OverUnderscanSupport(*iter & BITMASK_TRUE(2));
    result.it_scan_behaviour = OverUnderscanSupport(*iter >> 2 & BITMASK_TRUE(2));
    result.pt_scan_behaviour = OverUnderscanSupport(*iter >> 4 & BITMASK_TRUE(2));
    result.is_rgb_quantization_range_selectable = *iter >> 6 & BITMASK_TRUE(1);
    result.is_ycc_quantization_range_selectable = *iter >> 7 & BITMASK_TRUE(1);

    return result;
  }

  std::vector<uint8_t> VideoCapabilityDataBlock::generate_byte_block() const {
    std::vector<uint8_t> result(size(), 0x00);
    int pos = 0;

    result[pos] = CTA861_EXTENDED_TAG << 5;
    result[pos++] |= (size() - CTA861_DATA_BLOCK_HEADER_SIZE) & BITMASK_TRUE(5);
    result[pos++] = CTA861_EXTENDED_VIDEO_CAPABILITY_BLOCK_TAG;

    result[pos] |= is_ycc_quantization_range_selectable << 7;
    result[pos] |= is_rgb_quantization_range_selectable << 6;
    result[pos] |= (pt_scan_behaviour & BITMASK_TRUE(2)) << 4;
    result[pos] |= (it_scan_behaviour & BITMASK_TRUE(2)) << 2;
    result[pos] |= ce_scan_behaviour & BITMASK_TRUE(2);

    return result;
  }

  void VideoCapabilityDataBlock::print(std::ostream& os, uint8_t tabs) const {
    std::string indent(tabs, '\t');
    os << indent << "Video Capability Data Block: \n";
    indent += '\t';

    os << indent << "CE scan support: " << ce_scan_behaviour;
    os << indent << "IT scan support: " << it_scan_behaviour;
    os << indent << "PT scan support: " << pt_scan_behaviour;
    os << indent << "RGB Quantization Range selectable: " << is_rgb_quantization_range_selectable;
    os << indent << "YCC Quantization Range selectable: " << is_ycc_quantization_range_selectable;
  }
}  // namespace Edid
