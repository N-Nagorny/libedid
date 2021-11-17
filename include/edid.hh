#pragma once

#include <optional>
#include <vector>

#include "base_block.hh"
#include "cta861_block.hh"

namespace Edid {
  struct EdidData {
    BaseBlock base_block;
    std::optional<std::vector<Cta861Block>> extension_blocks;
  };

  std::vector<uint8_t> generate_edid_binary(const EdidData& edid);
}
