#include <array>
#include <cstdint>
#include <iostream>

#include "common.hh"
#include "cta861_block.hh"
#include "exceptions.hh"

namespace Edid {
  std::vector<uint8_t> generate_data_block_collection(const DataBlockCollection& collection) {
    std::vector<uint8_t> result(collection.size(), 0x0);

    // Video Data Block
    int pos = 0;

    result[pos] = CTA861_VIDEO_DATA_BLOCK_TAG << 5;
    result[pos++] |= collection.video_data_block.valid_vics() & 0x1F;

    for (const std::optional<uint8_t>& vic : collection.video_data_block.vics)
      if (vic.has_value())
        result[pos++] = vic.value();

    // Audio Data Block
    result[pos] = CTA861_AUDIO_DATA_BLOCK_TAG << 5;
    result[pos++] |= collection.short_audio_descriptor.size() & 0x1F;

    result[pos] = collection.short_audio_descriptor.audio_format << 3;
    result[pos++] |= collection.short_audio_descriptor.channels;

    result[pos++] = collection.short_audio_descriptor.sampling_freqs;

    result[pos++] = collection.short_audio_descriptor.lpcm_bit_depths;

    // Speaker Allocation Block
    result[pos] = CTA861_SPEAKERS_DATA_BLOCK_TAG << 5;
    result[pos++] |= 0x03;

    result[pos++] = 0;
    result[pos++] = collection.speaker_allocation;
    result[pos++] = 0;
    result[pos++] = 0;

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
}
