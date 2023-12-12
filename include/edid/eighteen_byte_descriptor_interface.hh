// Copyright 2023 N-Nagorny
#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>

#define EIGHTEEN_BYTES 18

namespace Edid {
  struct IEighteenByteDescriptor {
    virtual ~IEighteenByteDescriptor() = default;

    virtual uint8_t type() const = 0;
    virtual std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const = 0;
    virtual void print(std::ostream& os, uint8_t tabs = 1) const = 0;
  };
}  // namespace Edid
