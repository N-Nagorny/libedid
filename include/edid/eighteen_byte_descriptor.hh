// Copyright 2023 N-Nagorny
#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>

#include "dtd.hh"
#include "eighteen_byte_descriptors.hh"

namespace Edid {
  template<typename T>
  concept EighteenByteDescriptorInterface = std::is_base_of_v<IEighteenByteDescriptor, T>;

  template<EighteenByteDescriptorInterface... Ts>
  using EighteenByteDescriptorVariant = std::variant<Ts...>;

  using EighteenByteDescriptor = EighteenByteDescriptorVariant<
    DummyDescriptor,            // [E-EDID] Section 3.10.3.11
    DetailedTimingDescriptor,   // [E-EDID] Section 3.10.2
    DisplayRangeLimits,         // [E-EDID] Section 3.10.3.3
    AsciiString,                // [E-EDID] Sections 3.10.3.1, 3.10.3.2, 3.10.3.4
    EstablishedTimings3         // [E-EDID] Section 3.10.3.9
  >;

  static bool has_18_byte_descr_type(const EighteenByteDescriptor& descriptor, uint8_t type) {
    return std::visit([type](auto&& descr){ return descr.type() == type; }, descriptor);
  }
}  // namespace Edid
