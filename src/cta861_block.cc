// Copyright 2023 N-Nagorny
#include <array>
#include <cstdint>
#include <iostream>
#include <numeric>

#include "edid/common.hh"
#include "edid/cta861_block.hh"

namespace Edid {
  bool operator==(const Cta861Block& lhs, const Cta861Block& rhs) {
    return std::tie(lhs.underscan, lhs.basic_audio, lhs.ycbcr_444, lhs.ycbcr_422, lhs.data_block_collection, lhs.detailed_timing_descriptors) ==
      std::tie(rhs.underscan, rhs.basic_audio, rhs.ycbcr_444, rhs.ycbcr_422, rhs.data_block_collection, rhs.detailed_timing_descriptors);
  }

  std::vector<uint8_t> generate_data_block_collection(const DataBlockCollection& collection) {
    size_t collection_size = std::accumulate(collection.begin(), collection.end(), 0, [](size_t size, const CtaDataBlock& data_block) {
      return size + std::visit(get_cta_data_block_size, data_block);
    });
    std::vector<uint8_t> result(collection_size, 0x0);
    auto pos = result.begin();

    for (const auto& data_block : collection) {
      auto cta_data_block_visitor =
        [](const auto& cta_data_block) -> std::vector<uint8_t> {
          return cta_data_block.generate_byte_block();
        };

      std::vector<uint8_t> byte_block = std::visit(cta_data_block_visitor, data_block);
      pos = std::move(byte_block.begin(), byte_block.end(), pos);
    }

    return result;
  }

  std::array<uint8_t, EDID_BLOCK_SIZE> generate_cta861_block(const Cta861Block& cta861) {
    const int header_size = 4;
    const int checksum_size = 1;
    const int dtd_block_size = cta861.detailed_timing_descriptors.size() * EIGHTEEN_BYTES;
    const int data_block_collection_size = std::accumulate(cta861.data_block_collection.begin(), cta861.data_block_collection.end(), 0, [](size_t size, const CtaDataBlock& data_block) {
      return size + std::visit(get_cta_data_block_size, data_block);
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

    int dtd_start_pos = 0x4 + data_block_collection_size;
    if (cta861.detailed_timing_descriptors.empty() && data_block_collection_size == 0)
      dtd_start_pos = 0x0;

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
      auto dtd = detailed_timing_descriptor.generate_byte_block();
      std::move(dtd.begin(), dtd.end(), result.begin() + pos);
      pos += dtd.size();
    }

    while (pos < EDID_BLOCK_SIZE - 1)
      result[pos++] = 0x0;

    result[pos] = calculate_block_checksum(result);

    return result;
  }

  template<typename Iterator>
  std::unique_ptr<CtaDataBlock> parse_extended_tag_data_block(Iterator iter_read) {
    std::unique_ptr<CtaDataBlock> data_block_ptr = nullptr;

    uint8_t extended_tag = *(iter_read + 1);
    switch (extended_tag) {
      case CTA861_EXTENDED_YCBCR420_CAPABILITY_MAP_DATA_BLOCK_TAG:
        data_block_ptr = std::make_unique<CtaDataBlock>(std::move(YCbCr420CapabilityMapDataBlock::parse_byte_block(iter_read)));
        break;
      default:
        data_block_ptr = std::make_unique<CtaDataBlock>(std::move(UnknownDataBlock::parse_byte_block(iter_read)));
    }
    return data_block_ptr;
  }

  template<typename Iterator>
  std::unique_ptr<CtaDataBlock> parse_vendor_specific_data_block(Iterator iter_read) {
    std::unique_ptr<CtaDataBlock> data_block_ptr = nullptr;

    const std::array<uint8_t, 3> oui = {*(iter_read + 1), *(iter_read + 2), *(iter_read + 3)};
    if (oui == hdmi_oui_little_endian) {
      data_block_ptr = std::make_unique<CtaDataBlock>(std::move(HdmiVendorDataBlock::parse_byte_block(iter_read)));
    }
    else {
      data_block_ptr = std::make_unique<CtaDataBlock>(std::move(UnknownDataBlock::parse_byte_block(iter_read)));
    }
    return data_block_ptr;
  }

  DataBlockCollection parse_data_block_collection(const std::vector<uint8_t>& collection) {
    DataBlockCollection result;
    auto iter_read = collection.begin();
    std::unique_ptr<CtaDataBlock> data_block_ptr = nullptr;

    while (iter_read < collection.end()) {
      uint8_t data_block_tag = *iter_read >> 5 & BITMASK_TRUE(3);
      switch (data_block_tag) {
        case CTA861_VIDEO_DATA_BLOCK_TAG:
          data_block_ptr = std::make_unique<CtaDataBlock>(std::move(VideoDataBlock::parse_byte_block(iter_read)));
          break;
        case CTA861_AUDIO_DATA_BLOCK_TAG:
          data_block_ptr = std::make_unique<CtaDataBlock>(std::move(AudioDataBlock::parse_byte_block(iter_read)));
          break;
        case CTA861_SPEAKERS_DATA_BLOCK_TAG:
          data_block_ptr = std::make_unique<CtaDataBlock>(std::move(SpeakerAllocationDataBlock::parse_byte_block(iter_read)));
          break;
        case CTA861_EXTENDED_TAG:
          data_block_ptr = parse_extended_tag_data_block(iter_read);
          break;
        case CTA861_VENDOR_DATA_BLOCK_TAG:
          data_block_ptr = parse_vendor_specific_data_block(iter_read);
          break;
        default:
          data_block_ptr = std::make_unique<CtaDataBlock>(std::move(UnknownDataBlock::parse_byte_block(iter_read)));
      }
      iter_read += std::visit([](const auto& data_block) {
        return data_block.size();
      }, *data_block_ptr);
      result.push_back(*data_block_ptr);
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
        std::array<uint8_t, EIGHTEEN_BYTES> dtd_binary;
        std::move(
          cta861.begin() + pos,
          cta861.begin() + pos + EIGHTEEN_BYTES,
          dtd_binary.begin()
        );
        result.detailed_timing_descriptors.push_back(DetailedTimingDescriptor::parse_byte_block(dtd_binary));
        pos += EIGHTEEN_BYTES;
      }
    }

    return result;
  }

  void print_data_block_collection(std::ostream& os, const DataBlockCollection& collection) {
    for (const auto& data_block : collection) {
      std::visit([&os](const auto& d) {
        d.print(os);
      }, data_block);
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
      detailed_timing_descriptor.print(os);
    }
  }
}  // namespace Edid
