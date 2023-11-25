// Copyright 2023 N-Nagorny
#include <cstdint>
#include <iostream>
#include <vector>

#include "edid/base_block.hh"
#include "edid/cta861_block.hh"
#include "edid/edid.hh"
#include "edid/exceptions.hh"

namespace Edid {
  bool operator==(const EdidData& lhs, const EdidData& rhs) {
    return std::tie(lhs.base_block, lhs.extension_blocks) ==
      std::tie(rhs.base_block, rhs.extension_blocks);
  }

  std::vector<uint8_t> generate_edid_binary(const EdidData& edid) {
    int ext_blocks = edid.extension_blocks.value_or(std::vector<Cta861Block>{}).size();
    size_t edid_size = (ext_blocks + 1) * EDID_BLOCK_SIZE;
    std::vector<uint8_t> result(edid_size, 0x0);

    auto base_block = generate_base_block(edid.base_block, ext_blocks);
    std::move(base_block.begin(), base_block.end(), result.begin());

    for (int i = 0; i < ext_blocks; ++i) {
      auto cta861_block = generate_cta861_block(edid.extension_blocks->at(i));
      std::move(cta861_block.begin(), cta861_block.end(), result.begin() + (i + 1) * EDID_BLOCK_SIZE);
    }

    return result;
  }

  EdidData parse_edid_binary(const std::vector<uint8_t>& edid) {
    if (edid.size() == 0 || edid.size() % EDID_BLOCK_SIZE != 0)
      throw EdidException(__FUNCTION__, "EDID size " + std::to_string(edid.size()) +
        " is not a factor of " +
        std::to_string(EDID_BLOCK_SIZE)
      );

    EdidData result;
    std::array<uint8_t, EDID_BLOCK_SIZE> base;
    std::move(edid.begin(), edid.begin() + EDID_BLOCK_SIZE, base.begin());
    auto [base_edid, extension_blocks] = parse_base_block(base);

    result.base_block = base_edid;
    if (extension_blocks != 0)
      result.extension_blocks = std::vector<Cta861Block>();
    for (int i = 0; i < extension_blocks; ++i) {
      std::array<uint8_t, EDID_BLOCK_SIZE> extension_block;
      std::move(
        edid.begin() + (i + 1) * EDID_BLOCK_SIZE,
        edid.begin() + (i + 2) * EDID_BLOCK_SIZE,
        extension_block.begin()
      );
      result.extension_blocks->push_back(parse_cta861_block(extension_block));
    }

    return result;
  }
}  // namespace Edid
