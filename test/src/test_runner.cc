#include <fstream>
#include <iostream>
#include <optional>

#include "gtest/gtest.h"

#include "edid/base_block.hh"
#include "edid/edid.hh"


#include "common.hh"

using namespace Edid;

TEST(EqualityOperatorTests, DetailedTimingDescriptorIsEqualToItself) {
  DetailedTimingDescriptor dtd = DetailedTimingDescriptor{
    148'500'000, 1920, 1080, 280, 45, 88, 44,
    4, 5, 1039, 584, 0, 0, DtdFeaturesBitmap{false, NO_STEREO, DigitalSeparateSync{true, true}}
  };
  EXPECT_EQ(dtd, dtd);
}

TEST(EqualityOperatorTests, DisplayRangeLimitsAreEqualToThemselves) {
  DisplayRangeLimits limits = DisplayRangeLimits{56, 75, 30, 83, 170};
  EXPECT_EQ(limits, limits);
}

TEST(EqualityOperatorTests, StandardTimingIsEqualToItself) {
  StandardTiming std_timing = StandardTiming{1152, AspectRatio::AR_4_3, 75};
  EXPECT_EQ(std_timing, std_timing);
}

TEST(EqualityOperatorTests, EdidBaseIsEqualToItself) {
  EXPECT_EQ(make_edid_base(), make_edid_base());
}

TEST(CircularTests, BaseEdid) {
  EdidData edid;
  edid.base_block = make_edid_base();
  auto edid_binary = generate_edid_binary(edid);
  std::array<uint8_t, EDID_BLOCK_SIZE> base_edid_binary;
  std::move(edid_binary.begin(), edid_binary.begin() + EDID_BLOCK_SIZE, base_edid_binary.begin());
  EXPECT_EQ(parse_base_block(base_edid_binary).first, make_edid_base());
}

TEST(CircularTests, DetailedTimingDescriptor) {
  DetailedTimingDescriptor dtd = DetailedTimingDescriptor{
    148'500'000, 1920, 1080, 280, 45, 88, 44,
    4, 5, 1039, 584, 0, 0, DtdFeaturesBitmap{false, NO_STEREO, DigitalSeparateSync{true, true}}
  };
  auto binary = make_dtd(dtd);
  EXPECT_EQ(dtd, parse_dtd(binary));
}

TEST(CircularTests, VideoDataBlock) {
  VideoDataBlock video_data_block;
  video_data_block.vics[0] = 16;
  video_data_block.vics[1] = 4;
  video_data_block.vics[2] = 31;
  video_data_block.vics[3] = 19;
  video_data_block.vics[4] = 2;
  video_data_block.vics[5] = 18;
  video_data_block.vics[6] = 1;
  std::vector<uint8_t> vdb_binary = generate_video_data_block(video_data_block);
  EXPECT_EQ(video_data_block, parse_video_data_block(vdb_binary));
}

TEST(CircularTests, AudioDataBlock) {
  ShortAudioDescriptor sad;
  sad.audio_format = AudioFormatCode::LPCM;
  sad.channels = AudioChannels::AC_2;
  sad.sampling_freqs |= SamplingFrequence::SF_48;
  sad.sampling_freqs |= SamplingFrequence::SF_44_1;
  sad.sampling_freqs |= SamplingFrequence::SF_32;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_16;
  AudioDataBlock audio_data_block;
  audio_data_block.sads[0] = sad;
  std::vector<uint8_t> adb_binary = generate_audio_data_block(audio_data_block);
  EXPECT_EQ(audio_data_block, parse_audio_data_block(adb_binary));
}

TEST(CircularTests, Cta861Block) {
  std::array<uint8_t, EDID_BLOCK_SIZE> cta861_binary = generate_cta861_block(make_cta861_ext());
  EXPECT_EQ(make_cta861_ext(), parse_cta861_block(cta861_binary));
}

TEST(CircularTests, FullEdid) {
  EdidData edid{make_edid_base(), std::vector{make_cta861_ext()}};
  auto edid_binary = generate_edid_binary(edid);
  EXPECT_EQ(edid, parse_edid_binary(edid_binary));
}

TEST(WildEdidParsing, KoganKaled24144F_HDMI) {
  uint8_t edid_binary[] = {
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x2c, 0xee, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00,
    0x1c, 0x1a, 0x01, 0x04, 0xa2, 0x34, 0x20, 0x78, 0x2e, 0x61, 0x50, 0xa6, 0x56, 0x50, 0xa0, 0x26,
    0x0d, 0x50, 0x54, 0xaf, 0xcb, 0x00, 0xb3, 0x00, 0x95, 0x00, 0x81, 0x80, 0x81, 0xc0, 0x71, 0x4f,
    0xa9, 0xc0, 0x8b, 0xc0, 0x01, 0x00, 0x02, 0x3a, 0x80, 0x18, 0x71, 0x38, 0x2d, 0x40, 0x58, 0x2c,
    0x45, 0x00, 0xdd, 0x0c, 0x11, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x4b, 0x41, 0x4c,
    0x45, 0x44, 0x32, 0x34, 0x31, 0x34, 0x34, 0x46, 0x0a, 0x20, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x28,
    0x90, 0xa0, 0xa0, 0x21, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82,

    0x02, 0x03, 0x2b, 0xf1, 0x50, 0x90, 0x05, 0x04, 0x03, 0x02, 0x01, 0x11, 0x12, 0x13, 0x14, 0x06,
    0x07, 0x15, 0x16, 0x1f, 0x20, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00, 0x67, 0xd8, 0x5d,
    0xc4, 0x01, 0x78, 0xc0, 0x00, 0x65, 0x03, 0x0c, 0x00, 0x20, 0x00, 0x9a, 0x7f, 0x80, 0x78, 0x70,
    0x38, 0x20, 0x40, 0x58, 0x2c, 0x45, 0x00, 0x56, 0x50, 0x21, 0x00, 0x00, 0x18, 0x8a, 0x6f, 0x80,
    0xa0, 0x70, 0x38, 0x40, 0x40, 0x30, 0x20, 0x35, 0x00, 0x40, 0x44, 0x21, 0x00, 0x00, 0x1e, 0x29,
    0x36, 0x80, 0xa0, 0x70, 0x38, 0x1f, 0x40, 0x30, 0x20, 0x35, 0x00, 0x40, 0x44, 0x21, 0x00, 0x00,
    0x1e, 0x02, 0x19, 0x00, 0xa0, 0x50, 0xd0, 0x15, 0x20, 0x30, 0x20, 0x35, 0x00, 0x80, 0xd8, 0x10,
    0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17
  };

  BaseBlock edid_base;
  edid_base.manufacturer_id = {'K', 'G', 'N'};
  edid_base.manufacturer_product_code = 9216;
  edid_base.manufacture_week = 28;
  edid_base.manufacture_year = 2016;
  edid_base.bits_per_color = BitDepth::BD_8;
  edid_base.video_interface = VideoInterface::HDMIa;
  edid_base.h_screen_size = 52;
  edid_base.v_screen_size = 32;
  edid_base.display_type = DigitalDisplayType::RGB444_YCrCb444;
  edid_base.gamma = 120;
  edid_base.dpms_active_off = true;
  edid_base.standard_srgb = true;
  edid_base.preferred_timing_mode = true;

  edid_base.chromaticity = {0x61, 0x50, 0xa6, 0x56, 0x50, 0xa0, 0x26, 0x0d, 0x50, 0x54};

  edid_base.established_timings_1 |= EstablishedTiming1::ET_720x400_70;
  edid_base.established_timings_1 |= EstablishedTiming1::ET_640x480_60;
  edid_base.established_timings_1 |= EstablishedTiming1::ET_640x480_72;
  edid_base.established_timings_1 |= EstablishedTiming1::ET_640x480_75;
  edid_base.established_timings_1 |= EstablishedTiming1::ET_800x600_56;
  edid_base.established_timings_1 |= EstablishedTiming1::ET_800x600_60;

  edid_base.established_timings_2 |= EstablishedTiming2::ET_800x600_72;
  edid_base.established_timings_2 |= EstablishedTiming2::ET_800x600_75;
  edid_base.established_timings_2 |= EstablishedTiming2::ET_1024x768_60;
  edid_base.established_timings_2 |= EstablishedTiming2::ET_1024x768_75;
  edid_base.established_timings_2 |= EstablishedTiming2::ET_1280x1024_75;

  edid_base.standard_timings[0] = StandardTiming{1680, AspectRatio::AR_16_10, 60};
  edid_base.standard_timings[1] = StandardTiming{1440, AspectRatio::AR_16_10, 60};
  edid_base.standard_timings[2] = StandardTiming{1280, AspectRatio::AR_5_4, 60};
  edid_base.standard_timings[3] = StandardTiming{1280, AspectRatio::AR_16_9, 60};
  edid_base.standard_timings[4] = StandardTiming{1152, AspectRatio::AR_4_3, 75};
  edid_base.standard_timings[5] = StandardTiming{1600, AspectRatio::AR_16_9, 60};
  edid_base.standard_timings[6] = StandardTiming{1360, AspectRatio::AR_16_9, 60};

  edid_base.display_range_limits = DisplayRangeLimits{40, 144, 160, 160, 330};
  edid_base.detailed_timing_descriptors[0] = DetailedTimingDescriptor{
    148'500'000, 1920, 1080, 280, 45, 88, 44,
    4, 5, 477, 268, 0, 0, DtdFeaturesBitmap{false, NO_STEREO, DigitalSeparateSync{false, false}}
  };
  edid_base.display_name = "KALED24144F";

  Cta861Block cta861;
  cta861.underscan = true;
  cta861.basic_audio = true;
  cta861.ycbcr_444 = true;
  cta861.ycbcr_422 = true;
  cta861.data_block_collection.video_data_block.vics[0] = 16;
  cta861.data_block_collection.video_data_block.vics[1] = 5;
  cta861.data_block_collection.video_data_block.vics[2] = 4;
  cta861.data_block_collection.video_data_block.vics[3] = 3;
  cta861.data_block_collection.video_data_block.vics[4] = 2;
  cta861.data_block_collection.video_data_block.vics[5] = 1;
  cta861.data_block_collection.video_data_block.vics[6] = 17;
  cta861.data_block_collection.video_data_block.vics[7] = 18;
  cta861.data_block_collection.video_data_block.vics[8] = 19;
  cta861.data_block_collection.video_data_block.vics[9] = 20;
  cta861.data_block_collection.video_data_block.vics[10] = 6;
  cta861.data_block_collection.video_data_block.vics[11] = 7;
  cta861.data_block_collection.video_data_block.vics[12] = 21;
  cta861.data_block_collection.video_data_block.vics[13] = 22;
  cta861.data_block_collection.video_data_block.vics[14] = 31;
  cta861.data_block_collection.video_data_block.vics[15] = 32;

  ShortAudioDescriptor sad;
  sad.audio_format = AudioFormatCode::LPCM;
  sad.channels = AudioChannels::AC_2;
  sad.sampling_freqs |= SamplingFrequence::SF_48;
  sad.sampling_freqs |= SamplingFrequence::SF_44_1;
  sad.sampling_freqs |= SamplingFrequence::SF_32;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_16;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_20;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_24;
  cta861.data_block_collection.audio_data_block.sads[0] = sad;
  cta861.data_block_collection.speaker_allocation_data_block.speaker_allocation |= FRONT_LEFT_AND_RIGHT;

  cta861.detailed_timing_descriptors.push_back(DetailedTimingDescriptor{
    326'660'000, 1920, 1080, 120, 32, 88, 44,
    4, 5, 598, 336, 0, 0, DtdFeaturesBitmap{false, NO_STEREO, DigitalSeparateSync{false, false}}
  });
  cta861.detailed_timing_descriptors.push_back(DetailedTimingDescriptor{
    285'540'000, 1920, 1080, 160, 64, 48, 32,
    3, 5, 576, 324, 0, 0, DtdFeaturesBitmap{false, NO_STEREO, DigitalSeparateSync{true, true}}
  });
  cta861.detailed_timing_descriptors.push_back(DetailedTimingDescriptor{
    138'650'000, 1920, 1080, 160, 31, 48, 32,
    3, 5, 576, 324, 0, 0, DtdFeaturesBitmap{false, NO_STEREO, DigitalSeparateSync{true, true}}
  });
  cta861.detailed_timing_descriptors.push_back(DetailedTimingDescriptor{
    64'020'000, 1280, 720, 160, 21, 48, 32,
    3, 5, 384, 216, 0, 0, DtdFeaturesBitmap{false, NO_STEREO, DigitalSeparateSync{false, false}}
  });

  std::vector<uint8_t> edid = std::vector<uint8_t>(edid_binary, edid_binary + ARRAY_SIZE(edid_binary));
  EdidData edid_data{edid_base, std::vector{cta861}};
  EXPECT_EQ(edid_data, parse_edid_binary(edid));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
