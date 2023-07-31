#include <gtest/gtest.h>

#include "edid/bcp00501.hh"

#include "common.hh"

#define ARR(...) __VA_ARGS__

#define RATE(rate) \
  nlohmann::json({ \
    { "numerator", rate }, \
  })

#define RATE59_94 \
  nlohmann::json({ \
    { "numerator", 60000 }, \
    { "denominator", 1001 }, \
  })

#define VIDEO_MODE(width, height, frame_rates, interlace_modes) \
  nlohmann::json({ \
    { "urn:x-nmos:cap:format:grain_rate", {{ "enum", {frame_rates} }} }, \
    { "urn:x-nmos:cap:format:frame_width", {{ "enum", {width} }} }, \
    { "urn:x-nmos:cap:format:frame_height", {{ "enum", {height} }} }, \
    { "urn:x-nmos:cap:format:interlace_mode",  {{ "enum", {interlace_modes} }} }, \
  })

using namespace Edid;

TEST(Bcp00501Tests, BasicTest) {
  nlohmann::json j_expected = {
    VIDEO_MODE(1920, 1080, ARR(RATE(50)), ARR("progressive")),
    VIDEO_MODE(720, 480, ARR(RATE59_94), ARR("progressive")),
    VIDEO_MODE(640, 480, ARR({{ "denominator", 420000}, {"numerator",25175000} }), ARR("progressive")),
    VIDEO_MODE(1280, 720, ARR(RATE(50)), ARR("progressive")),
    VIDEO_MODE(1920, 1080, ARR(RATE(60)), ARR("progressive")),
    VIDEO_MODE(1152, 870, ARR(RATE(75)), ARR("progressive")),
    VIDEO_MODE(1024, 768, ARR(RATE(87)), ARR("interlaced_bff", "interlaced_psf", "interlaced_tff")),
    VIDEO_MODE(1280, 720, ARR(RATE(60)), ARR("progressive")),
    VIDEO_MODE(1024, 768, ARR(RATE(70)), ARR("progressive")),
    VIDEO_MODE(800, 600, ARR(RATE(56)), ARR("progressive")),
    VIDEO_MODE(1440, 900, ARR(RATE(60)), ARR("progressive")),
    VIDEO_MODE(1440, 900, ARR(RATE(75)), ARR("progressive")),
    VIDEO_MODE(1600, 1200, ARR(RATE(60)), ARR("progressive")),
    VIDEO_MODE(800, 600, ARR(RATE(85)), ARR("progressive")),
    VIDEO_MODE(720, 576, ARR(RATE(50)), ARR("progressive")),
    VIDEO_MODE(1152, 864, ARR(RATE(75)), ARR("progressive")),
    VIDEO_MODE(1280, 1024, ARR(RATE(60)), ARR("progressive")),
    VIDEO_MODE(1280, 800, ARR(RATE(60)), ARR("progressive")),
  };

  EdidData edid{make_edid_base(), std::vector<Cta861Block>{make_cta861_ext()}};

  auto j_actual = generate_constraint_sets(edid);

  std::sort(j_expected.begin(), j_expected.end());
  std::sort(j_actual.begin(), j_actual.end());

  EXPECT_EQ(j_expected, j_actual);
}
