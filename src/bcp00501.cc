#include <unordered_set>

#include "edid/bcp00501.hh"
#include "edid/timing_modes.hh"

using json = nlohmann::json;

namespace Edid {
  json generate_constraint_set(const VideoTimingMode& mode) {
    const std::string cap_frame_width = "urn:x-nmos:cap:format:frame_width";
    const std::string cap_frame_height = "urn:x-nmos:cap:format:frame_height";
    const std::string cap_grain_rate = "urn:x-nmos:cap:format:grain_rate";
    const std::string cap_interlace_mode = "urn:x-nmos:cap:format:interlace_mode";

    const std::string interlaced_bff = "interlaced_bff";
    const std::string interlaced_tff = "interlaced_tff";
    const std::string interlaced_psf = "interlaced_psf";
    const std::string progressive = "progressive";

    json result;

    result[cap_frame_width]["enum"] = { mode.h_res };
    result[cap_frame_height]["enum"] = { mode.v_res };
    if (mode.v_rate_hz.first % mode.v_rate_hz.second == 0) {
      result[cap_grain_rate]["enum"] = {{
        { "numerator", mode.v_rate_hz.first / mode.v_rate_hz.second },
      }};
    }
    else {
      uint64_t numerator = mode.v_rate_hz.first;
      uint64_t denominator = mode.v_rate_hz.second;

      if (mode.v_rate_hz.first % NTSC_FACTOR_NUMERATOR == 0 && mode.v_rate_hz.second % NTSC_FACTOR_DENOMINATOR == 0) {
        numerator /= NTSC_FACTOR_NUMERATOR;
        denominator /= NTSC_FACTOR_DENOMINATOR;
        numerator /= denominator;
        numerator *= NTSC_FACTOR_NUMERATOR;
        denominator = NTSC_FACTOR_DENOMINATOR;
      }

      result[cap_grain_rate]["enum"] = {{
        { "numerator", numerator },
        { "denominator", denominator },
      }};
    }
    if (mode.interlaced) {
      result[cap_interlace_mode]["enum"] = { interlaced_bff, interlaced_psf, interlaced_tff };
    }
    else {
      result[cap_interlace_mode]["enum"] = { progressive };
    }

    return result;
  }

  template <class Block>
  std::unordered_set<json> generate_constraint_sets(const Block& block) {
    std::unordered_set<json> result;

    for_each_mode(block, [&result](const VideoTimingMode& mode) {
      result.insert(generate_constraint_set(mode));
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
