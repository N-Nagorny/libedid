#include <cstdint>
#include <iostream>
#include <vector>

#include "base_block.hh"
#include "cta861_block.hh"
#include "edid.hh"

namespace Edid {
  std::vector<uint8_t> generate_edid_binary(const EdidData& edid) {
    int ext_blocks = edid.extension_blocks.value_or(std::vector<Cta861Block>{}).size();
    size_t edid_size = (ext_blocks + 1) * EDID_BLOCK_SIZE;
    std::vector<uint8_t> result(edid_size, 0x0);

    auto base_block = generate_base_block(edid.base_block, ext_blocks);
    std::move(base_block.begin(), base_block.end(), result.begin());

    if (ext_blocks != 0) {
      for (int i = 0; i < ext_blocks; ++i) {
        auto cta861_block = generate_cta861_block(edid.extension_blocks->at(i));
        std::move(cta861_block.begin(), cta861_block.end(), result.begin() + (i + 1) * EDID_BLOCK_SIZE);
      }
    }

    return result;
  }
}
