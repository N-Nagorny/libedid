#pragma once

#include <map>

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

// Extern declarations for auto-generated constants
namespace Edid {
  namespace json_schemas {
    using nlohmann::json_uri;
    using nlohmann::json;

    namespace base_block {
      extern const char* base_block;
      extern const char* standard_timing;
    }

    namespace common {
      extern const char* ascii_string;
      extern const char* detailed_timing_descriptor;
      extern const char* edid;
      extern const char* display_range_limits;
      extern const char* established_timings_3;
    }

    namespace cta861_block {
      extern const char* audio_data_block;
      extern const char* cta861_block;
      extern const char* speaker_allocation_data_block;
      extern const char* unknown_data_block;
      extern const char* video_data_block;
      extern const char* ycbcr420_capability_map_data_block;
    }

    static const std::map<json_uri, json> schemas = {
      { json_uri{"base_block.json"}, json::parse(base_block::base_block) },
      { json_uri{"standard_timing.json"}, json::parse(base_block::standard_timing) },

      { json_uri{"ascii_string.json"}, json::parse(common::ascii_string) },
      { json_uri{"detailed_timing_descriptor.json"}, json::parse(common::detailed_timing_descriptor) },
      { json_uri{"edid.json"}, json::parse(common::edid) },
      { json_uri{"display_range_limits.json"}, json::parse(common::display_range_limits) },
      { json_uri{"established_timings_3.json"}, json::parse(common::established_timings_3) },

      { json_uri{"audio_data_block.json"}, json::parse(cta861_block::audio_data_block) },
      { json_uri{"cta861_block.json"}, json::parse(cta861_block::cta861_block) },
      { json_uri{"speaker_allocation_data_block.json"}, json::parse(cta861_block::speaker_allocation_data_block) },
      { json_uri{"unknown_data_block.json"}, json::parse(cta861_block::unknown_data_block) },
      { json_uri{"video_data_block.json"}, json::parse(cta861_block::video_data_block) },
      { json_uri{"ycbcr420_capability_map_data_block.json"}, json::parse(cta861_block::ycbcr420_capability_map_data_block) },
    };

    static nlohmann::json_schema::schema_loader schema_loader = [](const json_uri& id, json& value) {
      auto found = schemas.find(id);

      if (found == schemas.end()) {
        throw std::runtime_error("JSON Schema not found for " + id.to_string());
      }

      value = found->second;
    };

    bool is_json_satisfies_schema(const std::string& uri, const json& j);
  }
}
