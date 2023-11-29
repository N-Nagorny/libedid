// Copyright 2023 N-Nagorny
#pragma once

#include <array>
#include <memory>
#include <optional>
#include <vector>

#include "common.hh"
#include "cta_data_block.hh"
#include "hdmi_vendor_data_block.hh"

#define CTA861_EXT_TAG 0x02
#define CTA861_VERSION 3

namespace Edid {
  using CtaDataBlock = std::variant<
    UnknownDataBlock, VideoDataBlock,
    AudioDataBlock, SpeakerAllocationDataBlock,
    YCbCr420CapabilityMapDataBlock, HdmiVendorDataBlock,
    ColorimetryDataBlock
  >;
  using DataBlockCollection = std::vector<CtaDataBlock>;

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

  std::vector<uint8_t> generate_data_block_collection(const DataBlockCollection& collection);
  std::array<uint8_t, EDID_BLOCK_SIZE> generate_cta861_block(const Cta861Block& cta861_block);

  DataBlockCollection parse_data_block_collection(const std::vector<uint8_t>& collection);
  Cta861Block parse_cta861_block(const std::array<uint8_t, EDID_BLOCK_SIZE>& cta861);

  void print_cta861_block(std::ostream& os, const Cta861Block& cta861_block);
}  // namespace Edid
