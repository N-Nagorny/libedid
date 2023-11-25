// Copyright 2023 N-Nagorny
#pragma once

#include "edid.hh"
#include "timing_modes.hh"

#include <nlohmann/json.hpp>

namespace Edid {
  nlohmann::json generate_constraint_set(const VideoTimingMode& mode);
  nlohmann::json generate_constraint_sets(const EdidData& edid);
}
