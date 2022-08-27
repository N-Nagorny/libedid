#include <gtest/gtest.h>

#include "edid/bcp00501.hh"

#include "common.hh"

using namespace Edid;

TEST(Bcp00501Tests, BasicTest) {
  nlohmann::json j = R"(
    [
        {
            "urn:x-nmos:cap:format:frame_height":{
                "enum":[
                    1080
                ]
            },
            "urn:x-nmos:cap:format:frame_width":{
                "enum":[
                    1920
                ]
            },
            "urn:x-nmos:cap:format:grain_rate":{
                "enum":[
                    {
                        "numerator":50
                    }
                ]
            },
            "urn:x-nmos:cap:format:interlace_mode":{
                "enum":[
                    "progressive"
                ]
            }
        },
        {
            "urn:x-nmos:cap:format:frame_height":{
                "enum":[
                    480
                ]
            },
            "urn:x-nmos:cap:format:frame_width":{
                "enum":[
                    720
                ]
            },
            "urn:x-nmos:cap:format:grain_rate":{
                "enum":[
                    {
                        "denominator":1001,
                        "numerator":60000
                    }
                ]
            },
            "urn:x-nmos:cap:format:interlace_mode":{
                "enum":[
                    "progressive"
                ]
            }
        },
        {
            "urn:x-nmos:cap:format:frame_height":{
                "enum":[
                    480
                ]
            },
            "urn:x-nmos:cap:format:frame_width":{
                "enum":[
                    640
                ]
            },
            "urn:x-nmos:cap:format:grain_rate":{
                "enum":[
                    {
                        "denominator":420000,
                        "numerator":25175000
                    }
                ]
            },
            "urn:x-nmos:cap:format:interlace_mode":{
                "enum":[
                    "progressive"
                ]
            }
        },
        {
            "urn:x-nmos:cap:format:frame_height":{
                "enum":[
                    720
                ]
            },
            "urn:x-nmos:cap:format:frame_width":{
                "enum":[
                    1280
                ]
            },
            "urn:x-nmos:cap:format:grain_rate":{
                "enum":[
                    {
                        "numerator":50
                    }
                ]
            },
            "urn:x-nmos:cap:format:interlace_mode":{
                "enum":[
                    "progressive"
                ]
            }
        },
        {
            "urn:x-nmos:cap:format:frame_height":{
                "enum":[
                    1080
                ]
            },
            "urn:x-nmos:cap:format:frame_width":{
                "enum":[
                    1920
                ]
            },
            "urn:x-nmos:cap:format:grain_rate":{
                "enum":[
                    {
                        "numerator":60
                    }
                ]
            },
            "urn:x-nmos:cap:format:interlace_mode":{
                "enum":[
                    "progressive"
                ]
            }
        },
        {
            "urn:x-nmos:cap:format:frame_height":{
                "enum":[
                    576
                ]
            },
            "urn:x-nmos:cap:format:frame_width":{
                "enum":[
                    720
                ]
            },
            "urn:x-nmos:cap:format:grain_rate":{
                "enum":[
                    {
                        "numerator":50
                    }
                ]
            },
            "urn:x-nmos:cap:format:interlace_mode":{
                "enum":[
                    "progressive"
                ]
            }
        },
        {
            "urn:x-nmos:cap:format:frame_height":{
                "enum":[
                    870
                ]
            },
            "urn:x-nmos:cap:format:frame_width":{
                "enum":[
                    1152
                ]
            },
            "urn:x-nmos:cap:format:grain_rate":{
                "enum":[
                    {
                        "numerator":75
                    }
                ]
            },
            "urn:x-nmos:cap:format:interlace_mode":{
                "enum":[
                    "progressive"
                ]
            }
        },
        {
            "urn:x-nmos:cap:format:frame_height":{
                "enum":[
                    768
                ]
            },
            "urn:x-nmos:cap:format:frame_width":{
                "enum":[
                    1024
                ]
            },
            "urn:x-nmos:cap:format:grain_rate":{
                "enum":[
                    {
                        "numerator":87
                    }
                ]
            },
            "urn:x-nmos:cap:format:interlace_mode":{
                "enum":[
                    "interlaced_bff",
                    "interlaced_psf",
                    "interlaced_tff"
                ]
            }
        },
        {
            "urn:x-nmos:cap:format:frame_height":{
                "enum":[
                    720
                ]
            },
            "urn:x-nmos:cap:format:frame_width":{
                "enum":[
                    1280
                ]
            },
            "urn:x-nmos:cap:format:grain_rate":{
                "enum":[
                    {
                        "numerator":60
                    }
                ]
            },
            "urn:x-nmos:cap:format:interlace_mode":{
                "enum":[
                    "progressive"
                ]
            }
        },
        {
            "urn:x-nmos:cap:format:frame_height":{
                "enum":[
                    768
                ]
            },
            "urn:x-nmos:cap:format:frame_width":{
                "enum":[
                    1024
                ]
            },
            "urn:x-nmos:cap:format:grain_rate":{
                "enum":[
                    {
                        "numerator":70
                    }
                ]
            },
            "urn:x-nmos:cap:format:interlace_mode":{
                "enum":[
                    "progressive"
                ]
            }
        },
        {
            "urn:x-nmos:cap:format:frame_height":{
                "enum":[
                    600
                ]
            },
            "urn:x-nmos:cap:format:frame_width":{
                "enum":[
                    800
                ]
            },
            "urn:x-nmos:cap:format:grain_rate":{
                "enum":[
                    {
                        "numerator":56
                    }
                ]
            },
            "urn:x-nmos:cap:format:interlace_mode":{
                "enum":[
                    "progressive"
                ]
            }
        }
    ]
  )"_json;

  EdidData edid{make_edid_base(), std::vector<Cta861Block>{make_cta861_ext()}};

  EXPECT_EQ(j, generate_constraint_sets(edid));
}
