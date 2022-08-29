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
      extern const char* detailed_timing_descriptor;
      extern const char* display_name;
      extern const char* display_range_limits;
      extern const char* display_serial_number;
    }

    namespace cta861_block {
      extern const char* audio_data_block;
      extern const char* cta861_block;
      extern const char* speaker_allocation_data_block;
      extern const char* unknown_data_block;
      extern const char* video_data_block;
    }

    static const std::map<json_uri, json> schemas = {
      { json_uri{"base_block/base_block.json"}, json::parse(base_block::base_block) },
      { json_uri{"base_block/standard_timing.json"}, json::parse(base_block::standard_timing) },

      { json_uri{"common/detailed_timing_descriptor.json"}, json::parse(common::detailed_timing_descriptor) },
      { json_uri{"common/display_name.json"}, json::parse(common::display_name) },
      { json_uri{"common/display_range_limits.json"}, json::parse(common::display_range_limits) },
      { json_uri{"common/display_serial_number.json"}, json::parse(common::display_serial_number) },

      { json_uri{"cta861_block/audio_data_block.json"}, json::parse(cta861_block::audio_data_block) },
      { json_uri{"cta861_block/cta861_block.json"}, json::parse(cta861_block::cta861_block) },
      { json_uri{"cta861_block/speaker_allocation_data_block.json"}, json::parse(cta861_block::speaker_allocation_data_block) },
      { json_uri{"cta861_block/unknown_data_block.json"}, json::parse(cta861_block::unknown_data_block) },
      { json_uri{"cta861_block/video_data_block.json"}, json::parse(cta861_block::video_data_block) },
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
