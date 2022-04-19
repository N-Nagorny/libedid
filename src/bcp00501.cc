#include <unordered_set>

#include "edid/bcp00501.hh"
#include "edid/timing_modes.hh"

using json = nlohmann::json;

namespace Edid {
  using Ratio = std::pair<uint64_t, uint64_t>;

  json generate_constraint_set(uint16_t frame_width, uint16_t frame_height, Ratio frame_rate, bool interlaced) {
    const std::string cap_frame_width = "urn:x-nmos:cap:format:frame_width";
    const std::string cap_frame_height = "urn:x-nmos:cap:format:frame_height";
    const std::string cap_grain_rate = "urn:x-nmos:cap:format:grain_rate";
    const std::string cap_interlace_mode = "urn:x-nmos:cap:format:interlace_mode";

    json result;

    result[cap_frame_width]["enum"] = { frame_width };
    result[cap_frame_height]["enum"] = { frame_height };
    if (frame_rate.first % frame_rate.second == 0) {
      result[cap_grain_rate]["enum"]["numerator"] = frame_rate.first / frame_rate.second;
    }
    else {
      uint64_t numerator = frame_rate.first;
      uint64_t denominator = frame_rate.second;

      if (frame_rate.first % NTSC_FACTOR_NUMERATOR == 0 && frame_rate.second % NTSC_FACTOR_DENOMINATOR == 0) {
        numerator /= NTSC_FACTOR_NUMERATOR;
        denominator /= NTSC_FACTOR_DENOMINATOR;
        numerator /= denominator;
        numerator *= NTSC_FACTOR_NUMERATOR;
        denominator = NTSC_FACTOR_DENOMINATOR;
      }

      result[cap_grain_rate]["enum"] = {
        { "numerator", numerator },
        { "denominator", denominator },
      };
    }
    result[cap_interlace_mode]["enum"] = { interlaced };

    return result;
  }

  template <class Block>
  std::unordered_set<json> generate_constraint_sets(const Block& block) {
    std::unordered_set<json> result;

    for_each_mode(block, [&result](uint16_t frame_width, uint16_t frame_height, Ratio frame_rate, bool interlaced) {
      result.insert(generate_constraint_set(frame_width, frame_height, frame_rate, interlaced));
    });

    return result;
  }

  json generate_constraint_sets(const EdidData& edid) {
    std::unordered_set<json> constraint_sets = generate_constraint_sets(edid.base_block);
    if (edid.extension_blocks.has_value()) {
      for (const auto& ext_block : edid.extension_blocks.value()) {
        std::unordered_set<json> temp = generate_constraint_sets(ext_block);
        std::move(temp.begin(), temp.end(), std::inserter(constraint_sets, constraint_sets.end()));
      }
    }
    return json(constraint_sets);
  }
}
