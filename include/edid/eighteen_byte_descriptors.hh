// Copyright 2023 N-Nagorny
#pragma once

#include <array>
#include <cstdint>
#include <iostream>
#include <memory>

#include "eighteen_byte_descriptor_interface.hh"

#define BASE_DISPLAY_DESCRIPTOR_HEADER_SIZE 5
#define MAX_ASCII_STRING_LENGTH 13

#define BASE_FAKE_DTD_TYPE 0xF6  // reserved so it should be safe (Section 3.10.3.10)
#define BASE_DISPLAY_DESCRIPTOR_DUMMY_TYPE 0x10
#define BASE_DISPLAY_DESCRIPTOR_ESTABLISHED_TIMINGS_III_TYPE 0xF7
#define BASE_DISPLAY_DESCRIPTOR_RANGE_LIMITS_TYPE 0xFD

namespace Edid {
  enum VideoTimingSupport {
    VTS_DEFAULT_GTF = 0x00,
    VTS_BARE_LIMITS = 0x01,
    VTS_SECONDARY_GTF = 0x02,
    VTS_CVT = 0x03
  };

  STRINGIFY_ENUM(VideoTimingSupport, {
    {VTS_DEFAULT_GTF, "GTF"},
    {VTS_BARE_LIMITS, "Bare Limits"},
    {VTS_SECONDARY_GTF, "Secondary GTF"},
    {VTS_CVT, "CVT"},
  })

  struct DisplayRangeLimits : IEighteenByteDescriptor {
    uint16_t min_v_rate_hz = 1;  // Range is 1..510 Hz with step of 1 Hz
    uint16_t max_v_rate_hz = 1;  // Range is 1..510 Hz with step of 1 Hz
    uint16_t min_h_rate_khz = 1;  // Range is 1..510 kHz with step of 1 kHz
    uint16_t max_h_rate_khz = 1;  // Range is 1..510 kHz with step of 1 kHz
    uint16_t max_pixel_clock_rate_mhz = 10;  // Range is 10..2550 MHz with step of 10 MHz
    VideoTimingSupport video_timing_support = VideoTimingSupport::VTS_DEFAULT_GTF;

    DisplayRangeLimits() = default;

    // for brace-enclosed initialization despite
    // inheritance from a base class with virtual funcs
    DisplayRangeLimits(
      uint16_t min_v_rate_hz_,
      uint16_t max_v_rate_hz_,
      uint16_t min_h_rate_khz_,
      uint16_t max_h_rate_khz_,
      uint16_t max_pixel_clock_rate_mhz_,
      VideoTimingSupport video_timing_support_
    )
      : min_v_rate_hz(min_v_rate_hz_)
      , max_v_rate_hz(max_v_rate_hz_)
      , min_h_rate_khz(min_h_rate_khz_)
      , max_h_rate_khz(max_h_rate_khz_)
      , max_pixel_clock_rate_mhz(max_pixel_clock_rate_mhz_)
      , video_timing_support(video_timing_support_)
    {}

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DisplayRangeLimits,
      min_v_rate_hz,
      max_v_rate_hz,
      min_h_rate_khz,
      max_h_rate_khz,
      max_pixel_clock_rate_mhz,
      video_timing_support
    )
#endif

    uint8_t type() const override {
      return BASE_DISPLAY_DESCRIPTOR_RANGE_LIMITS_TYPE;
    }

    std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    static DisplayRangeLimits parse_byte_block(const uint8_t* start) {
      DisplayRangeLimits result;
      int pos = 0;

      bool add_255_to_h_max = false;
      bool add_255_to_h_min = false;
      bool add_255_to_v_max = false;
      bool add_255_to_v_min = false;
      pos += BASE_DISPLAY_DESCRIPTOR_HEADER_SIZE - 1;  // Only for Display Range Limits Descriptor
      if (*(start + pos) >> 2 & BITMASK_TRUE(2) == 0b10) {
        add_255_to_h_max = true;
      } else if (*(start + pos) >> 2 & BITMASK_TRUE(2) == 0b11) {
        add_255_to_h_max = true;
        add_255_to_h_min = true;
      }
      if (*(start + pos) & BITMASK_TRUE(2) == 0b10) {
        add_255_to_v_max = true;
      } else if (*(start + pos) & BITMASK_TRUE(2) == 0b11) {
        add_255_to_v_max = true;
        add_255_to_v_min = true;
      }
      result.min_v_rate_hz = *(start + ++pos);
      result.max_v_rate_hz = *(start + ++pos);
      result.min_h_rate_khz = *(start + ++pos);
      result.max_h_rate_khz = *(start + ++pos);
      if (add_255_to_v_min)
        result.min_v_rate_hz += 255;
      if (add_255_to_v_max)
        result.max_v_rate_hz += 255;
      if (add_255_to_h_min)
        result.min_h_rate_khz += 255;
      if (add_255_to_h_max)
        result.max_h_rate_khz += 255;
      result.max_pixel_clock_rate_mhz = *(start + ++pos) * 10;
      result.video_timing_support = VideoTimingSupport(*(start + ++pos));
      return result;
    }
  };

#define FIELDS(X) X.min_v_rate_hz, X.max_v_rate_hz, X.min_h_rate_khz, X.max_h_rate_khz, \
                  X.max_pixel_clock_rate_mhz, X.video_timing_support
  TIED_COMPARISONS(DisplayRangeLimits, FIELDS)
#undef FIELDS

  enum AsciiStringType {
    ASCII_DISPLAY_NAME = 0xFC,
    ASCII_UNSPECIFIED_TEXT = 0xFE,
    ASCII_SERIAL_NUMBER = 0xFF
  };

  STRINGIFY_ENUM(AsciiStringType, {
    {ASCII_DISPLAY_NAME,      "Display Name"},
    {ASCII_UNSPECIFIED_TEXT,  "Unspecified Text"},
    {ASCII_SERIAL_NUMBER,     "Serial Number"}
  })

  struct AsciiString : IEighteenByteDescriptor {
    std::string string;  // The maximum is 13 chars
    AsciiStringType descriptor_type = AsciiStringType::ASCII_UNSPECIFIED_TEXT;

    AsciiString() = default;

    // for brace-enclosed initialization despite
    // inheritance from a base class with virtual funcs
    AsciiString(
      const std::string& string_,
      AsciiStringType descriptor_type_
    )
      : string(string_)
      , descriptor_type(descriptor_type_)
    {}

#ifdef ENABLE_JSON
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AsciiString, string, descriptor_type)
#endif

    uint8_t type() const override {
      return descriptor_type;
    }

    std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    static AsciiString parse_byte_block(const uint8_t* start) {
      AsciiString result;
      start += 3;
      result.descriptor_type = AsciiStringType(*start);
      start += 2;

      for (int i = 0; i < MAX_ASCII_STRING_LENGTH; ++i) {
        if (*(start + i) != '\n')
          result.string.push_back(*(start + i));
        else
          break;
      }
      return result;
    };
  };

#define FIELDS(X) X.string, X.descriptor_type
  TIED_COMPARISONS(AsciiString, FIELDS)
#undef FIELDS

  struct DummyDescriptor : IEighteenByteDescriptor {
    std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    uint8_t type() const override {
      return BASE_DISPLAY_DESCRIPTOR_DUMMY_TYPE;
    }
  };

  inline bool operator==(const DummyDescriptor& lhs, const DummyDescriptor& rhs) {
    return true;
  }

  struct EstablishedTimings3 : IEighteenByteDescriptor {
    std::array<uint8_t, 6> bytes_6_11{};

    std::array<uint8_t, EIGHTEEN_BYTES> generate_byte_block() const override;
    void print(std::ostream& os, uint8_t tabs = 1) const override;

    uint8_t type() const override {
      return BASE_DISPLAY_DESCRIPTOR_ESTABLISHED_TIMINGS_III_TYPE;
    }

    static EstablishedTimings3 parse_byte_block(const uint8_t* start) {
      EstablishedTimings3 result;
      start += BASE_DISPLAY_DESCRIPTOR_HEADER_SIZE;
      start++;

      for (int i = 0; i < 6; ++i) {
        result.bytes_6_11[i] = *(start + i);
      }
      return result;
    };
  };

#define FIELDS(X) X.bytes_6_11
  TIED_COMPARISONS(EstablishedTimings3, FIELDS)
#undef FIELDS

}  // namespace Edid
