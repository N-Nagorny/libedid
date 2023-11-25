// Copyright 2023 N-Nagorny
#include "edid/json_schemas.hh"

namespace Edid {
  namespace json_schemas {
    bool is_json_satisfies_schema(const std::string& uri, const nlohmann::json& j) {
      try {
        nlohmann::json_schema::json_validator validator{
          schemas.at(nlohmann::json_uri{uri}),
          schema_loader
        };
        validator.validate(j);
      } catch (const std::exception &e) {
        return false;
      }
      return true;
    }
  }  // namespace json_schemas
}  // namespace Edid
