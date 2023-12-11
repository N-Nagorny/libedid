// Copyright 2023 N-Nagorny
#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>

#define EIGHTEEN_BYTES 18

#define BASE_FAKE_DTD_TYPE 0xF6  // reserved so it should be safe (Section 3.10.3.10)
#define BASE_DISPLAY_DESCRIPTOR_DUMMY_TYPE 0x10
#define BASE_DISPLAY_DESCRIPTOR_ESTABLISHED_TIMINGS_III_TYPE 0xF7
#define BASE_DISPLAY_DESCRIPTOR_RANGE_LIMITS_TYPE 0xFD

namespace Edid {
  struct IEighteenByteDescriptor {
    virtual ~IEighteenByteDescriptor() = default;

    virtual uint8_t type() const = 0;
    virtual std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const = 0;
    virtual void print(std::ostream& os, uint8_t tabs = 1) const = 0;

    static std::shared_ptr<IEighteenByteDescriptor> parse_byte_block(const uint8_t* start);
  };
}  // namespace Edid
