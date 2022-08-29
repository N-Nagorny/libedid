#include <fstream>

#include <gtest/gtest.h>

#include "edid/json.hh"
#include "edid/json_schemas.hh"

#include "common.hh"

using namespace Edid;
using Edid::json_schemas::is_json_satisfies_schema;

TEST(JsonTests, DtdCircularTest) {
  nlohmann::json j = R"(
    {
      "h_blanking":280,
      "h_border":0,
      "h_front_porch":88,
      "h_image_size":1039,
      "h_res":1920,
      "h_sync_width":44,
      "interlaced":false,
      "pixel_clock":14850,
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

TEST(JsonTests, StandardTimingCircularTest) {
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

TEST(JsonTests, BaseBlockCircularTest) {
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
                "pixel_clock":14850,
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
                "display_product_name":"Hello"
            }
        ],
        "established_timings_1":[
            "800x600@56Hz"
        ],
        "established_timings_2":[
            "1024x768@70Hz",
            "1024x768@87Hz interlaced"
        ],
        "established_timings_3":[
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
        "week_of_manufacture":2,
        "year_of_manufacture":2020
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

TEST(ShortAudioDescriptorTests, JsonCircularTest) {
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
            "pixel_clock":14850,
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
            "display_product_name":"Hello"
          }
        ],
        "established_timings_1":[
          "800x600@56Hz"
        ],
        "established_timings_2":[
          "1024x768@70Hz",
          "1024x768@87Hz interlaced"
        ],
        "established_timings_3":[
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
        "week_of_manufacture":2,
        "year_of_manufacture":2020
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
              "pixel_clock":14850,
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

TEST(JsonTests, Cta861CircularTest) {
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
          "pixel_clock":14850,
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
