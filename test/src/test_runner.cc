#include <fstream>
#include <iostream>
#include <optional>

#include "gtest/gtest.h"

#include "base_block.hh"
#include "edid.hh"

using namespace Edid;

BaseBlock make_edid_base() {
  BaseBlock edid_base;
  edid_base.manufacturer_id = {'A', 'B', 'C'};
  edid_base.manufacturer_product_code = 1234;
  edid_base.manufacture_week = 2;
  edid_base.manufacture_year = 2020;
  edid_base.bits_per_color = BitDepth::BD_8;
  edid_base.display_type = DigitalDisplayType::RGB444_YCrCb444;
  edid_base.gamma = 1;
  edid_base.chromaticity = {0xEE, 0x91, 0xA3, 0x54, 0x4C, 0x99, 0x26, 0x0F, 0x50, 0x54};
  edid_base.established_timings_1 |= EstablishedTiming1::ET_800x600_56;
  edid_base.established_timings_2 |= EstablishedTiming2::ET_1024x768_70;
  edid_base.established_timings_3 |= EstablishedTiming3::ET_1152x870_75;
  edid_base.standard_timings[0] = StandardTiming{1152, AspectRatio::AR_4_3, 75};
  edid_base.standard_timings[1] = StandardTiming{1280, AspectRatio::AR_5_4, 60};
  edid_base.standard_timings[2] = StandardTiming{1280, AspectRatio::AR_16_10, 60};
  edid_base.standard_timings[3] = StandardTiming{1440, AspectRatio::AR_16_10, 60};
  edid_base.standard_timings[4] = StandardTiming{1920, AspectRatio::AR_16_9, 60};
  edid_base.display_range_limits = DisplayRangeLimits{56, 75, 30, 83, 170};
  edid_base.detailed_timing_descriptors[0] = DetailedTimingDescriptor{
    138'500'000, 1920, 1080, 28, 22, 88, 44,
    4, 5, 960, 540, 25, 21, 0x1E
  };
  edid_base.display_name = "Hello";
  return edid_base;
}

Cta861Block make_cta861_ext() {
  Cta861Block cta861;
  cta861.underscan = true;
  cta861.basic_audio = true;
  cta861.ycbcr_444 = true;
  cta861.ycbcr_422 = true;
  cta861.data_block_collection.video_data_block.vics[0] = 16;
  cta861.data_block_collection.video_data_block.vics[1] = 4;
  cta861.data_block_collection.video_data_block.vics[2] = 31;
  cta861.data_block_collection.video_data_block.vics[3] = 19;
  cta861.data_block_collection.video_data_block.vics[4] = 2;
  cta861.data_block_collection.video_data_block.vics[5] = 18;
  cta861.data_block_collection.video_data_block.vics[6] = 1;
  ShortAudioDescriptor sad;
  sad.audio_format = AudioFormatCode::LPCM;
  sad.channels = AudioChannels::AC_2;
  sad.sampling_freqs |= SamplingFrequence::SF_48;
  sad.sampling_freqs |= SamplingFrequence::SF_44_1;
  sad.sampling_freqs |= SamplingFrequence::SF_32;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_16;
  cta861.data_block_collection.audio_data_block.sads[0] = sad;
  cta861.detailed_timing_descriptors.push_back(DetailedTimingDescriptor{
    138'500'000, 1920, 1080, 28, 22, 88, 44,
    4, 5, 960, 540, 25, 21, 0x1E
  });
  return cta861;
}

TEST(EqualityOperatorTests, DetailedTimingDescriptorIsEqualToItself) {
  DetailedTimingDescriptor dtd = DetailedTimingDescriptor{
    138'500'000, 1920, 1080, 28, 22, 88, 44,
    4, 5, 960, 540, 25, 21, 0x1E
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
    138'500'000, 1920, 1080, 28, 22, 88, 44,
    4, 5, 960, 540, 25, 21, 0x1E
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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
