// Copyright 2023 N-Nagorny
#include <fstream>

#include <gtest/gtest.h>

#include "edid/json.hh"
#include "edid/json_schemas.hh"

#include "common.hh"

using namespace Edid;
using Edid::json_schemas::is_json_satisfies_schema;

TEST(JsonTests, DtdRoundtrip) {
  nlohmann::json j = R"(
    {
      "h_blanking":280,
      "h_border":0,
      "h_front_porch":88,
      "h_image_size":1039,
      "h_res":1920,
      "h_sync_width":44,
      "interlaced":false,
      "pixel_clock_khz":148500,
      "stereo_mode":"NO_STEREO",
      "sync":{
        "h_sync_polarity":true,
        "v_sync_polarity":true
      },
      "v_blanking":45,
      "v_border":0,
      "v_front_porch":4,
      "v_image_size":584,
      "v_res":1080,
      "v_sync_width":5
    }
  )"_json;

  DetailedTimingDescriptor dtd = j;
  nlohmann::json new_j = dtd;
  EXPECT_EQ(j, new_j);
  EXPECT_TRUE(
    is_json_satisfies_schema("detailed_timing_descriptor.json", new_j)
  );
}

TEST(JsonTests, StandardTimingRoundtrip) {
  nlohmann::json j = R"(
    {
      "aspect_ratio":"4:3",
      "v_frequency":75,
      "x_resolution":1152
    }
  )"_json;

  StandardTiming timing = j;
  nlohmann::json new_j = timing;
  EXPECT_EQ(j, new_j);
  EXPECT_TRUE(
    is_json_satisfies_schema("standard_timing.json", new_j)
  );
}

TEST(JsonTests, BaseBlockRoundtrip) {
  nlohmann::json j = R"(
    {
        "bit_depth":"8",
        "chromaticity":[
            238,
            145,
            163,
            84,
            76,
            153,
            38,
            15,
            80,
            84
        ],
        "continuous_timings":false,
        "digital_display_type":"RGB 4:4:4, YCrCb 4:4:4",
        "dpms_active_off":false,
        "dpms_standby":false,
        "dpms_suspend":false,
        "edid_major_version":1,
        "edid_minor_version":4,
        "eighteen_byte_descriptors":[
            {
                "h_blanking":280,
                "h_border":0,
                "h_front_porch":88,
                "h_image_size":1039,
                "h_res":1920,
                "h_sync_width":44,
                "interlaced":false,
                "pixel_clock_khz":148500,
                "stereo_mode":"NO_STEREO",
                "sync":{
                    "h_sync_polarity":true,
                    "v_sync_polarity":true
                },
                "v_blanking":45,
                "v_border":0,
                "v_front_porch":4,
                "v_image_size":584,
                "v_res":1080,
                "v_sync_width":5
            },
            {
                "max_h_rate_khz":83,
                "max_pixel_clock_rate_mhz":170,
                "max_v_rate_hz":75,
                "min_h_rate_khz":30,
                "min_v_rate_hz":56,
                "video_timing_support":"Bare Limits"
            },
            {
                "string":"Hello",
                "descriptor_type": "Unspecified Text"
            },
            {
                "established_timings_3": [
                    "800x600@85Hz",
                    "1440x900@75Hz",
                    "1440x900@60Hz",
                    "1600x1200@60Hz"
                ]
            }
        ],
        "established_timings_1":[
            "800x600@56Hz"
        ],
        "established_timings_2":[
            "1024x768@70Hz",
            "1024x768i@87Hz"
        ],
        "manufacturers_timings":[
            "1152x870@75Hz"
        ],
        "gamma":1,
        "h_screen_size":0,
        "manufacturer_id":"ABC",
        "preferred_timing_mode":true,
        "product_code":1234,
        "serial_number":0,
        "srgb_is_primary":false,
        "standard_timings":[
            {
               "aspect_ratio":"4:3",
               "v_frequency":75,
               "x_resolution":1152
            },
            {
               "aspect_ratio":"5:4",
               "v_frequency":60,
               "x_resolution":1280
            },
            {
               "aspect_ratio":"16:10",
               "v_frequency":60,
               "x_resolution":1280
            },
            {
               "aspect_ratio":"16:10",
               "v_frequency":60,
               "x_resolution":1440
            },
            {
               "aspect_ratio":"16:9",
               "v_frequency":60,
               "x_resolution":1920
            }
        ],
        "v_screen_size":0,
        "video_interface":"Undefined",
        "manufacture_date_or_model_year": {
          "week_of_manufacture":2,
          "year_of_manufacture":2020
        }
    }
  )"_json;

  nlohmann::json json = make_edid_base();
  BaseBlock base_block = json;

  EXPECT_EQ(j, json);
  EXPECT_EQ(base_block, make_edid_base());
  EXPECT_TRUE(
    is_json_satisfies_schema("base_block.json", json)
  );
}

TEST(ShortAudioDescriptorTests, JsonRoundtrip) {
  nlohmann::json j = R"(
    {
      "audio_format":"L-PCM",
      "channels":1,
      "lpcm_bit_depths":[
        20
      ],
      "sampling_freqs":[
        "32 kHz",
        "44.1 kHz",
        "48 kHz"
      ]
    }
  )"_json;
  ShortAudioDescriptor sad = j;
  nlohmann::json j1 = sad;
  EXPECT_EQ(j, j1);
}

TEST(ShortAudioDescriptorTests, Parsing) {
  nlohmann::json j = R"(
    {
      "audio_format":"L-PCM",
      "channels": 2,
      "lpcm_bit_depths":[
        20
      ],
      "sampling_freqs":[
        "32 kHz",
        "44.1 kHz",
        "48 kHz"
      ]
    }
  )"_json;
  ShortAudioDescriptor sad;
  sad.audio_format = AudioFormatCode::LPCM;
  sad.channels = AudioChannels::AC_2;
  sad.sampling_freqs |= SamplingFrequence::SF_48;
  sad.sampling_freqs |= SamplingFrequence::SF_44_1;
  sad.sampling_freqs |= SamplingFrequence::SF_32;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_20;

  ShortAudioDescriptor sad1 = j;
  EXPECT_EQ(sad, sad1);
}

TEST(ShortAudioDescriptorTests, Generating) {
  nlohmann::json j = R"(
    {
      "audio_format":"L-PCM",
      "channels": 2,
      "lpcm_bit_depths":[
        20
      ],
      "sampling_freqs":[
        "32 kHz",
        "44.1 kHz",
        "48 kHz"
      ]
    }
  )"_json;
  ShortAudioDescriptor sad;
  sad.audio_format = AudioFormatCode::LPCM;
  sad.channels = AudioChannels::AC_2;
  sad.sampling_freqs |= SamplingFrequence::SF_48;
  sad.sampling_freqs |= SamplingFrequence::SF_44_1;
  sad.sampling_freqs |= SamplingFrequence::SF_32;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_20;

  nlohmann::json j1 = sad;
  EXPECT_EQ(j, j1);
}

TEST(UnknownDataBlockTests, Generating) {
  nlohmann::json reserved_expected = R"(
    {
      "raw_data": [1, 2, 3, 4, 5],
      "data_block_tag": 0,
      "extended_tag": null
    }
  )"_json;
  UnknownDataBlock reserved = {
    {1, 2, 3, 4, 5},
    0
  };
  nlohmann::json reserved_actual = reserved;

  nlohmann::json extended_reserved_expected = R"(
    {
      "raw_data": [1, 2, 3, 4, 5],
      "data_block_tag": 7,
      "extended_tag": 128
    }
  )"_json;
  UnknownDataBlock extended_reserved = {
    {1, 2, 3, 4, 5},
    7,
    128
  };
  nlohmann::json extended_reserved_actual = extended_reserved;

  EXPECT_EQ(reserved_expected, reserved_actual);
  EXPECT_EQ(extended_reserved_expected, extended_reserved_actual);
}

TEST(JsonTests, FullEdidGenerating) {
  nlohmann::json j = R"(
    {
      "base_block":{
        "bit_depth":"8",
        "chromaticity":[
          238,
          145,
          163,
          84,
          76,
          153,
          38,
          15,
          80,
          84
        ],
        "continuous_timings":false,
        "digital_display_type":"RGB 4:4:4, YCrCb 4:4:4",
        "dpms_active_off":false,
        "dpms_standby":false,
        "dpms_suspend":false,
        "edid_major_version":1,
        "edid_minor_version":4,
        "eighteen_byte_descriptors":[
          {
            "h_blanking":280,
            "h_border":0,
            "h_front_porch":88,
            "h_image_size":1039,
            "h_res":1920,
            "h_sync_width":44,
            "interlaced":false,
            "pixel_clock_khz":148500,
            "stereo_mode":"NO_STEREO",
            "sync":{
              "h_sync_polarity":true,
              "v_sync_polarity":true
            },
            "v_blanking":45,
            "v_border":0,
            "v_front_porch":4,
            "v_image_size":584,
            "v_res":1080,
            "v_sync_width":5
          },
          {
            "max_h_rate_khz":83,
            "max_pixel_clock_rate_mhz":170,
            "max_v_rate_hz":75,
            "min_h_rate_khz":30,
            "min_v_rate_hz":56,
            "video_timing_support":"Bare Limits"
          },
          {
            "string":"Hello",
            "descriptor_type": "Unspecified Text"
          },
          {
            "established_timings_3": [
              "800x600@85Hz",
              "1440x900@75Hz",
              "1440x900@60Hz",
              "1600x1200@60Hz"
            ]
          }
        ],
        "established_timings_1":[
          "800x600@56Hz"
        ],
        "established_timings_2":[
          "1024x768@70Hz",
          "1024x768i@87Hz"
        ],
        "manufacturers_timings":[
          "1152x870@75Hz"
        ],
        "gamma":1,
        "h_screen_size":0,
        "manufacturer_id":"ABC",
        "preferred_timing_mode":true,
        "product_code":1234,
        "serial_number":0,
        "srgb_is_primary":false,
        "standard_timings":[
          {
            "aspect_ratio":"4:3",
            "v_frequency":75,
            "x_resolution":1152
          },
          {
            "aspect_ratio":"5:4",
            "v_frequency":60,
            "x_resolution":1280
          },
          {
            "aspect_ratio":"16:10",
            "v_frequency":60,
            "x_resolution":1280
          },
          {
            "aspect_ratio":"16:10",
            "v_frequency":60,
            "x_resolution":1440
          },
          {
            "aspect_ratio":"16:9",
            "v_frequency":60,
            "x_resolution":1920
          }
        ],
        "v_screen_size":0,
        "video_interface":"Undefined",
        "manufacture_date_or_model_year": {
          "week_of_manufacture":2,
          "year_of_manufacture":2020
        }
      },
      "extension_blocks":[
        {
          "basic_audio":true,
          "data_block_collection":[
            {
              "vics":[
                16,
                4,
                31,
                19,
                2,
                18,
                1
              ]
            },
            {
              "sads":[
                {
                  "audio_format":"L-PCM",
                  "channels":2,
                  "lpcm_bit_depths":[
                    16
                  ],
                  "sampling_freqs":[
                    "32 kHz",
                    "44.1 kHz",
                    "48 kHz"
                  ]
                }
              ]
            }
          ],
          "detailed_timing_descriptors":[
            {
              "h_blanking":280,
              "h_border":0,
              "h_front_porch":88,
              "h_image_size":1039,
              "h_res":1920,
              "h_sync_width":44,
              "interlaced":false,
              "pixel_clock_khz":148500,
              "stereo_mode":"NO_STEREO",
              "sync":{
                "h_sync_polarity":true,
                "v_sync_polarity":true
              },
              "v_blanking":45,
              "v_border":0,
              "v_front_porch":4,
              "v_image_size":584,
              "v_res":1080,
              "v_sync_width":5
            }
          ],
          "underscan":true,
          "ycbcr_422":true,
          "ycbcr_444":true
        }
      ]
    }
  )"_json;
  nlohmann::json json = EdidData{make_edid_base(), std::vector{make_cta861_ext()}};
  EXPECT_EQ(j, json);
  EXPECT_TRUE(
    is_json_satisfies_schema("edid.json", json)
  );
}

TEST(JsonTest, FullEdidParsing) {
  uint8_t edid_binary[] = {
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x4d, 0x10, 0xdb, 0x10, 0x00, 0x00, 0x00, 0x00,
    0xff, 0x17, 0x01, 0x03, 0x80, 0x85, 0x4b, 0x78, 0x2a, 0x1b, 0xbe, 0xa2, 0x55, 0x34, 0xb3, 0x26,
    0x14, 0x4a, 0x52, 0xaf, 0xce, 0x00, 0x90, 0x40, 0x81, 0x80, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3a, 0x80, 0xd0, 0x72, 0x38, 0x2d, 0x40, 0x10, 0x2c,
    0x45, 0x80, 0x31, 0xeb, 0x52, 0x00, 0x00, 0x1e, 0x66, 0x21, 0x50, 0xb0, 0x51, 0x00, 0x1b, 0x30,
    0x40, 0x70, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x53,
    0x48, 0x41, 0x52, 0x50, 0x20, 0x48, 0x44, 0x4d, 0x49, 0x0a, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfd,
    0x00, 0x17, 0x4c, 0x0e, 0x44, 0x0f, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xe2,

    0x02, 0x03, 0x28, 0x72, 0x50, 0x9f, 0x90, 0x20, 0x14, 0x05, 0x13, 0x04, 0x12, 0x03, 0x11, 0x02,
    0x16, 0x07, 0x15, 0x06, 0x01, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x67, 0x03, 0x0c,
    0x00, 0x20, 0x00, 0x80, 0x1e, 0xe2, 0x00, 0x3f, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38, 0x2d, 0x40,
    0x58, 0x2c, 0x45, 0x00, 0x31, 0xeb, 0x52, 0x00, 0x00, 0x1e, 0x01, 0x1d, 0x80, 0xd0, 0x72, 0x1c,
    0x16, 0x20, 0x10, 0x2c, 0x25, 0x80, 0x31, 0xeb, 0x52, 0x00, 0x00, 0x9e, 0x01, 0x1d, 0x80, 0x18,
    0x71, 0x1c, 0x16, 0x20, 0x58, 0x2c, 0x25, 0x00, 0x31, 0xeb, 0x52, 0x00, 0x00, 0x9e, 0x01, 0x1d,
    0x00, 0xbc, 0x52, 0xd0, 0x1e, 0x20, 0xb8, 0x28, 0x55, 0x40, 0x31, 0xeb, 0x52, 0x00, 0x00, 0x1e,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x57
  };

  std::vector<uint8_t> edid = std::vector<uint8_t>(edid_binary, edid_binary + ARRAY_SIZE(edid_binary));
  EXPECT_TRUE(
    is_json_satisfies_schema("edid.json", parse_edid_binary(edid))
  );
}

TEST(JsonTests, Cta861Roundtrip) {
  nlohmann::json j = R"(
    {
      "basic_audio":true,
      "data_block_collection":[
        {
          "vics":[
            16,
            4,
            31,
            19,
            2,
            18,
            1
          ]
        },
        {
          "sads":[
            {
              "audio_format":"L-PCM",
              "channels": 2,
              "lpcm_bit_depths":[ 16 ],
              "sampling_freqs":[
                "32 kHz",
                "44.1 kHz",
                "48 kHz"
              ]
            }
          ]
        }
      ],
      "detailed_timing_descriptors":[
        {
          "h_blanking":280,
          "h_border":0,
          "h_front_porch":88,
          "h_image_size":1039,
          "h_res":1920,
          "h_sync_width":44,
          "interlaced":false,
          "pixel_clock_khz":148500,
          "stereo_mode":"NO_STEREO",
          "sync":{
            "h_sync_polarity":true,
            "v_sync_polarity":true
          },
          "v_blanking":45,
          "v_border":0,
          "v_front_porch":4,
          "v_image_size":584,
          "v_res":1080,
          "v_sync_width":5
        }
      ],
      "underscan":true,
      "ycbcr_422":true,
      "ycbcr_444":true
    }
  )"_json;
  nlohmann::json json = make_cta861_ext();
  Cta861Block base_block = json;

  EXPECT_EQ(j, json);
  EXPECT_EQ(base_block, make_cta861_ext());
  EXPECT_TRUE(
    is_json_satisfies_schema("cta861_block.json", json)
  );
}

TEST(JsonTests, ColorimetryDataBlockGenerating) {
  const nlohmann::json expected = R"(
    {
      "colorimetry_standards": [
        "opRGB",
        "BT2020RGB",
        "defaultRGB",
        "ICtCp"
      ],
      "gamut_metadata_profiles": ["MD0"]
    }
  )"_json;

  ColorimetryDataBlock block{
    ENUM_NULL
    | CS_OP_RGB
    | CS_BT2020_RGB
    | CS_ICTCP
    | CS_DEFAULT_RGB,

    GMP_0
  };

  const nlohmann::json actual = block;
  EXPECT_EQ(expected, actual);
}

TEST(JsonTests, ColorimetryDataBlockParsing) {
  const nlohmann::json json = R"(
    {
      "colorimetry_standards": [
        "opRGB",
        "BT2020YCC",
        "xvYCC601",
        "defaultRGB",
        "ST2113RGB"
      ],
      "gamut_metadata_profiles": [ ]
    }
  )"_json;

  ColorimetryDataBlock expected{
    ENUM_NULL
    | CS_OP_RGB
    | CS_BT2020_YCC
    | CS_XV_YCC601
    | CS_DEFAULT_RGB
    | CS_ST2113_RGB,

    ENUM_NULL
  };

  ColorimetryDataBlock actual = json;

  EXPECT_EQ(actual, expected);
}

TEST(JsonTests, ColorimetryDataBlockRoundtrip) {
  ColorimetryDataBlock initial{
    ENUM_NULL
    | CS_OP_RGB
    | CS_BT2020_RGB
    | CS_ICTCP
    | CS_DEFAULT_RGB,

    GMP_0
  };

  const nlohmann::json json = initial;
  const ColorimetryDataBlock parsed = json;
  EXPECT_EQ(initial, parsed);
}
