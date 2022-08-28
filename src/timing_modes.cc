#include "edid/exceptions.hh"
#include "edid/timing_modes.hh"

namespace Edid {
  bool operator==(const VideoTimingMode& lhs, const VideoTimingMode& rhs) {
    return std::tie(lhs.h_res, lhs.v_res, lhs.v_rate_hz, lhs.interlaced) ==
      std::tie(rhs.h_res, rhs.v_res, rhs.v_rate_hz, rhs.interlaced);
  }

  #define N(x) uint8_t(x)
  #define R(x, y) std::make_pair<uint8_t, uint8_t>(x, y)
  #define V(...) std::vector<uint8_t>{__VA_ARGS__}

  static const std::vector<Cta861VideoTimingMode> cta_modes_1 = {
    /* VIC 1 */
    { {    25'175'000,   640,  480,  160,  45,   16,  96, 10,  2, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {    27'000'000,   720,  480,  138,  45,   16,  62,  9,  6, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {    27'000'000,   720,  480,  138,  45,   16,  62,  9,  6, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {    74'250'000,  1280,  720,  370,  30,  110,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1920,  540,  280,  22,   88,  44,  2,  5, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    27'000'000,  1440,  240,  276,  22,   38, 124,  4,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {    27'000'000,  1440,  240,  276,  22,   38, 124,  4,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {    27'000'000,  1440,  240,  276,  22,   38, 124,  4,  3, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {    27'000'000,  1440,  240,  276,  22,   38, 124,  4,  3, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {    54'000'000,  2880,  240,  552,  22,   76, 248,  4,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },   R(1, 10) },
    /* VIC 11 */
    { {    54'000'000,  2880,  240,  552,  22,   76, 248,  4,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },   R(1, 10) },
    { {    54'000'000,  2880,  240,  552,  22,   76, 248,  4,  3, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },   R(1, 10) },
    { {    54'000'000,  2880,  240,  552,  22,   76, 248,  4,  3, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },   R(1, 10) },
    { {    54'000'000,  1440,  480,  276,  45,   32, 124,  9,  6, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },    R(1, 2) },
    { {    54'000'000,  1440,  480,  276,  45,   32, 124,  9,  6, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },    R(1, 2) },
    { {   148'500'000,  1920, 1080,  280,  45,   88,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    27'000'000,   720,  576,  144,  49,   12,  64,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {    27'000'000,   720,  576,  144,  49,   12,  64,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {    74'250'000,  1280,  720,  700,  30,  440,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1920,  540,  720,  22,  528,  44,  2,  5, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    /* VIC 21 */
    { {    27'000'000,  1440,  288,  288,  24,   24, 126,  2,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {    27'000'000,  1440,  288,  288,  24,   24, 126,  2,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {    27'000'000,  1440,  288,  288,  24,   24, 126,  2,  3, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {    27'000'000,  1440,  288,  288,  24,   24, 126,  2,  3, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {    54'000'000,  2880,  288,  576,  24,   48, 252,  2,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },   R(1, 10) },
    { {    54'000'000,  2880,  288,  576,  24,   48, 252,  2,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },   R(1, 10) },
    { {    54'000'000,  2880,  288,  576,  24,   48, 252,  2,  3, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },   R(1, 10) },
    { {    54'000'000,  2880,  288,  576,  24,   48, 252,  2,  3, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },   R(1, 10) },
    { {    54'000'000,  1440,  576,  288,  49,   24, 128,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },    R(1, 2) },
    { {    54'000'000,  1440,  576,  288,  49,   24, 128,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },    R(1, 2) },
    /* VIC 31 */
    { {   148'500'000,  1920, 1080,  720,  45,  528,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1920, 1080,  830,  45,  638,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1920, 1080,  720,  45,  528,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1920, 1080,  280,  45,   88,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   108'000'000,  2880,  480,  552,  45,   64, 248,  9,  6, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } }, V(1, 2, 4) },
    { {   108'000'000,  2880,  480,  552,  45,   64, 248,  9,  6, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } }, V(1, 2, 4) },
    { {   108'000'000,  2880,  576,  576,  49,   48, 256,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } }, V(1, 2, 4) },
    { {   108'000'000,  2880,  576,  576,  49,   48, 256,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } }, V(1, 2, 4) },
    { {    72'000'000,  1920,  540,  384,  85,   32, 168, 23,  5, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{  true, false } } },       N(1) },
    { {   148'500'000,  1920,  540,  720,  22,  528,  44,  2,  5, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    /* VIC 41 */
    { {   148'500'000,  1280,  720,  700,  30,  440,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    54'000'000,   720,  576,  144,  49,   12,  64,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {    54'000'000,   720,  576,  144,  49,   12,  64,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {    54'000'000,  1440,  288,  288,  24,   24, 126,  2,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {    54'000'000,  1440,  288,  288,  24,   24, 126,  2,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {   148'500'000,  1920,  540,  280,  22,   88,  44,  2,  5, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   148'500'000,  1280,  720,  370,  30,  110,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    54'000'000,   720,  480,  138,  45,   16,  62,  9,  6, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {    54'000'000,   720,  480,  138,  45,   16,  62,  9,  6, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {    54'000'000,  1440,  240,  276,  22,   38, 124,  4,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    /* VIC 51 */
    { {    54'000'000,  1440,  240,  276,  22,   38, 124,  4,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {   108'000'000,   720,  576,  144,  49,   12,  64,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {   108'000'000,   720,  576,  144,  49,   12,  64,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {   108'000'000,  1440,  288,  288,  24,   24, 126,  2,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {   108'000'000,  1440,  288,  288,  24,   24, 126,  2,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {   108'000'000,   720,  480,  138,  45,   16,  62,  9,  6, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {   108'000'000,   720,  480,  138,  45,   16,  62,  9,  6, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(1) },
    { {   108'000'000,  1440,  240,  276,  22,   38, 124,  4,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {   108'000'000,  1440,  240,  276,  22,   38, 124,  4,  3, 0, 0, 0, 0, {  true, StereoMode::NO_STEREO, DigitalSeparateSync{ false, false } } },       N(2) },
    { {    59'400'000,  1280,  720, 2020,  30, 1760,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    /* VIC 61 */
    { {    74'250'000,  1280,  720, 2680,  30, 2420,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1280,  720, 2020,  30, 1760,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  1920, 1080,  280,  45,   88,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  1920, 1080,  720,  45,  528,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    59'400'000,  1280,  720, 2020,  30, 1760,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1280,  720, 2680,  30, 2420,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1280,  720, 2020,  30, 1760,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1280,  720,  700,  30,  440,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1280,  720,  370,  30,  110,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   148'500'000,  1280,  720,  700,  30,  440,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    /* VIC 71 */
    { {   148'500'000,  1280,  720,  370,  30,  110,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1920, 1080,  830,  45,  638,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1920, 1080,  720,  45,  528,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    74'250'000,  1920, 1080,  280,  45,   88,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   148'500'000,  1920, 1080,  720,  45,  528,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   148'500'000,  1920, 1080,  280,  45,   88,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  1920, 1080,  720,  45,  528,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  1920, 1080,  280,  45,   88,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    59'400'000,  1680,  720, 1620,  30, 1360,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    59'400'000,  1680,  720, 1488,  30, 1228,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    /* VIC 81 */
    { {    59'400'000,  1680,  720,  960,  30,  700,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    82'500'000,  1680,  720,  520,  30,  260,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    99'000'000,  1680,  720,  520,  30,  260,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   165'000'000,  1680,  720,  320, 105,   60,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   198'000'000,  1680,  720,  320, 105,   60,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    99'000'000,  2560, 1080, 1190,  20,  998,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    90'000'000,  2560, 1080,  640,  45,  448,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   118'800'000,  2560, 1080,  960,  45,  768,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   185'625'000,  2560, 1080,  740,  45,  548,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   198'000'000,  2560, 1080,  440,  20,  248,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    /* VIC 91 */
    { {   371'250'000,  2560, 1080,  410, 170,  218,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   495'000'000,  2560, 1080,  740, 170,  548,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  3840, 2160, 1660,  90, 1276,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  3840, 2160, 1440,  90, 1056,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  3840, 2160,  560,  90,  176,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   594'000'000,  3840, 2160, 1440,  90, 1056,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   594'000'000,  3840, 2160,  560,  90,  176,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  4096, 2160, 1404,  90, 1020,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  4096, 2160, 1184,  90,  968,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  4096, 2160,  304,  90,   88,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    /* VIC 101 */
    { {   594'000'000,  4096, 2160, 1184,  90,  968,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   594'000'000,  4096, 2160,  304,  90,   88,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  3840, 2160, 1660,  90, 1276,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  3840, 2160, 1440,  90, 1056,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   297'000'000,  3840, 2160,  560,  90,  176,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   594'000'000,  3840, 2160, 1440,  90, 1056,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   594'000'000,  3840, 2160,  560,  90,  176,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    90'000'000,  1280,  720, 1220,  30,  960,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    90'000'000,  1280,  720, 1220,  30,  960,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {    99'000'000,  1680,  720, 1070,  30,  810,  40,  5,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    /* VIC 111 */
    { {   148'500'000,  1920, 1080,  830,  45,  638,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   148'500'000,  1920, 1080,  830,  45,  638,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   198'000'000,  2560, 1080, 1190,  20,  998,  44,  4,  5, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   594'000'000,  3840, 2160, 1660,  90, 1276,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   594'000'000,  4096, 2160, 1404,  90, 1020,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   594'000'000,  3840, 2160, 1660,  90, 1276,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  3840, 2160, 1440,  90, 1056,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  3840, 2160,  560,  90,  176,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  3840, 2160, 1440,  90, 1056,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  3840, 2160,  560,  90,  176,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    /* VIC 121 */
    { {   396'000'000,  5120, 2160, 2380,  40, 1996,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   396'000'000,  5120, 2160, 2080,  40, 1696,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   396'000'000,  5120, 2160,  880,  40,  664,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   742'500'000,  5120, 2160, 1130, 315,  746,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   742'500'000,  5120, 2160, 1480,  90, 1096,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { {   742'500'000,  5120, 2160,  380,  90,  164,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'485'000'000,  5120, 2160, 1480,  90, 1096,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
  };

  static const std::vector<Cta861VideoTimingMode> cta_modes_2 = {
    /* VIC 193 */
    { { 1'485'000'000,  5120, 2160,  380,  90,  164,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  7680, 4320, 3320, 180, 2552, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  7680, 4320, 3120,  80, 2352, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  7680, 4320, 1320,  80,  552, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 2'376'000'000,  7680, 4320, 3320, 180, 2552, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 2'376'000'000,  7680, 4320, 3120,  80, 2352, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 2'376'000'000,  7680, 4320, 1320,  80,  552, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 4'752'000'000,  7680, 4320, 2880, 180, 2112, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },

    /* VIC 201 */
    { { 4'752'000'000,  7680, 4320, 1120, 180,  352, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  7680, 4320, 3320, 180, 2552, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  7680, 4320, 3120,  80, 2352, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  7680, 4320, 1320,  80,  552, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 2'376'000'000,  7680, 4320, 3320, 180, 2552, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 2'376'000'000,  7680, 4320, 3120,  80, 2352, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 2'376'000'000,  7680, 4320, 1320,  80,  552, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 4'752'000'000,  7680, 4320, 2880, 180, 2112, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 4'752'000'000,  7680, 4320, 1120, 180,  352, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'485'000'000, 10240, 4320, 2260, 630, 1492, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },

    /* VIC 211 */
    { { 1'485'000'000, 10240, 4320, 3260,  80, 2492, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'485'000'000, 10240, 4320,  760, 180,  288, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 2'970'000'000, 10240, 4320, 2260, 630, 1492, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 2'970'000'000, 10240, 4320, 3260,  80, 2492, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 2'970'000'000, 10240, 4320,  760, 180,  288, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 5'940'000'000, 10240, 4320, 2960, 180, 2192, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 5'940'000'000, 10240, 4320,  760, 180,  288, 176, 16, 20, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  4096, 2160, 1184,  90,  800,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
    { { 1'188'000'000,  4096, 2160,  304,  90,   88,  88,  8, 10, 0, 0, 0, 0, { false, StereoMode::NO_STEREO, DigitalSeparateSync{  true,  true } } },       N(1) },
  };

  #undef N
  #undef R
  #undef V

  Cta861VideoTimingMode get_cta861_video_timing_mode(uint8_t vic) {
    if (vic >= 1 && vic <= 127) {
      return cta_modes_1.at(vic - 1);
    }
    else if (vic >= 129 && vic <= 192) {
      return cta_modes_1.at(vic - 128 - 1);
    }
    else if (vic >= 193 && vic <= 219) {
      return cta_modes_2.at(vic - 193);
    }
    throw EdidException(std::to_string(vic) + " is not a valid VIC");
  }

  VideoTimingMode to_video_timing_mode(const DetailedTimingDescriptor& dtd, uint8_t pixel_repetition_factor) {
    return VideoTimingMode{
      static_cast<uint16_t>(dtd.h_res / pixel_repetition_factor),
      static_cast<uint16_t>(dtd.v_res * (dtd.features_bitmap.interlaced ? 2 : 1)),
      std::make_pair(dtd.pixel_clock_hz, (dtd.v_res + dtd.v_blanking) * (dtd.h_res + dtd.h_blanking)),
      dtd.features_bitmap.interlaced
    };
  }
}
