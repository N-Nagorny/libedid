// Copyright 2023 N-Nagorny
#pragma once

#include <stdexcept>
#include <string>

namespace Edid {
  class EdidException : public std::runtime_error {
   public:
    explicit EdidException(const std::string& what_arg)
      : std::runtime_error(what_arg) {};
    explicit EdidException(const char* what_arg)
      : std::runtime_error(what_arg) {};
    explicit EdidException(const char* func_name, const std::string& what_arg)
      : std::runtime_error(std::string(func_name) + ": " + what_arg) {};
    explicit EdidException(const char* func_name, const char* what_arg)
      : std::runtime_error(std::string(func_name) + ": " + std::string(what_arg)) {};
  };
}  // namespace Edid
