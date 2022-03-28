#pragma once

#include "edid.hh"

#include <nlohmann/json.hpp>

namespace Edid {
  nlohmann::json generate_constraint_sets(const EdidData& edid);
}
