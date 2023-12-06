// Copyright 2023 N-Nagorny
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

#define FIELDS(X) X.base_block, X.extension_blocks
  TIED_OP(EdidData, ==, FIELDS)
#undef FIELDS

  std::vector<uint8_t> generate_edid_binary(const EdidData& edid);
  EdidData parse_edid_binary(const std::vector<uint8_t>& edid);
}  // namespace Edid
