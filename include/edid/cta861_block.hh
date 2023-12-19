// Copyright 2023 N-Nagorny
#pragma once

#include <array>
#include <memory>
#include <optional>
#include <vector>

#include "cta_data_block.hh"
#include "dtd.hh"
#include "hdmi_vendor_data_block.hh"

#define CTA861_EXT_TAG 0x02
#define CTA861_VERSION 3

namespace Edid {
#if (defined(__cplusplus) && __cplusplus >= 202002L)
  template<typename T>
  concept CtaDataBlockInterface = std::is_base_of_v<ICtaDataBlock, T>;

  template<CtaDataBlockInterface... Ts>
  using CtaDataBlockVariant = std::variant<Ts...>;

  using CtaDataBlock = CtaDataBlockVariant<
#else
  using CtaDataBlock = std::variant<
#endif
    UnknownDataBlock,
    VideoDataBlock,                   // [CTA-861-I] Section 7.5.1
    AudioDataBlock,                   // [CTA-861-I] Section 7.5.2
    SpeakerAllocationDataBlock,       // [CTA-861-I] Section 7.5.3
    YCbCr420CapabilityMapDataBlock,   // [CTA-861-I] Section 7.5.11
    HdmiVendorDataBlock,              // [HDMI1.4b] Section 8.3.2
    HdrStaticMetadataDataBlock,       // [CTA-861-I] Section 7.5.13
    VideoCapabilityDataBlock,         // [CTA-861-I] Section 7.5.6
    ColorimetryDataBlock              // [CTA-861-I] Section 7.5.5
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

#define FIELDS(X) X.underscan, X.basic_audio, X.ycbcr_444, X.ycbcr_422, \
                  X.data_block_collection, X.detailed_timing_descriptors
  TIED_COMPARISONS(Cta861Block, FIELDS)
#undef FIELDS

  std::vector<uint8_t> generate_data_block_collection(const DataBlockCollection& collection);
  std::array<uint8_t, EDID_BLOCK_SIZE> generate_cta861_block(const Cta861Block& cta861_block);

  DataBlockCollection parse_data_block_collection(const std::vector<uint8_t>& collection);
  Cta861Block parse_cta861_block(const std::array<uint8_t, EDID_BLOCK_SIZE>& cta861);

  void print_cta861_block(std::ostream& os, const Cta861Block& cta861_block);
}  // namespace Edid
