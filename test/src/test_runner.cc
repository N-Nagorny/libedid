// Copyright 2023 N-Nagorny
#include <fstream>
#include <iostream>
#include <optional>

#include <gtest/gtest.h>

#include "edid/base_block.hh"
#include "edid/edid.hh"

#include "edid/timing_modes.hh"

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
  DisplayRangeLimits limits = DisplayRangeLimits{56, 75, 30, 83, 170, VideoTimingSupport::VTS_BARE_LIMITS};
  EXPECT_EQ(limits, limits);
}

TEST(EqualityOperatorTests, StandardTimingIsEqualToItself) {
  StandardTiming std_timing = StandardTiming{1152, AspectRatio::AR_4_3, 75};
  EXPECT_EQ(std_timing, std_timing);
}

TEST(EqualityOperatorTests, EdidBaseIsEqualToItself) {
  EXPECT_EQ(make_edid_base(), make_edid_base());
}

TEST(EqualityOperatorTests, Cta861BlockIsEqualToItself) {
  EXPECT_EQ(make_cta861_ext(), make_cta861_ext());
}

TEST(DisplayRangeLimits, Generating) {
  std::array<uint8_t, EIGHTEEN_BYTES> drl_binary{
    0x00, 0x00, 0x00, 0xfd, 0x00, 0x18, 0x4b, 0x0f, 0x8c, 0x32, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
  };
  DisplayRangeLimits limits = DisplayRangeLimits{24, 75, 15, 140, 500, VideoTimingSupport::VTS_DEFAULT_GTF};
  EXPECT_EQ(drl_binary, limits.generate_byte_block());
}

TEST(DisplayRangeLimits, Parsing) {
  std::array<uint8_t, EIGHTEEN_BYTES> drl_binary{
    0x00, 0x00, 0x00, 0xfd, 0x00, 0x18, 0x4b, 0x0f, 0x8c, 0x32, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
  };
  DisplayRangeLimits limits = DisplayRangeLimits{24, 75, 15, 140, 500, VideoTimingSupport::VTS_DEFAULT_GTF};
  EXPECT_EQ(DisplayRangeLimits::parse_byte_block(drl_binary.begin()), limits);
}

TEST(DisplayRangeLimits, Roundtrip) {
  DisplayRangeLimits limits = DisplayRangeLimits{56, 75, 30, 83, 170, VideoTimingSupport::VTS_BARE_LIMITS};
  auto limits_binary = limits.generate_byte_block();
  auto limits_parsed = DisplayRangeLimits::parse_byte_block(limits_binary.begin());
  EXPECT_EQ(limits, limits_parsed);
}

TEST(YCbCr420CapabilityMapDataBlockTests, Generating) {
  std::vector<uint8_t> capability_map_block = {0xe3, 0x0f, 0x83, 0x01};
  YCbCr420CapabilityMapDataBlock block;
  block.svd_indices = {1, 2, 8, 9};
  auto edid_binary = block.generate_byte_block();
  EXPECT_EQ(capability_map_block, edid_binary);
}

TEST(YCbCr420CapabilityMapDataBlockTests, Parsing) {
  std::vector<uint8_t> capability_map_block = {0xe4, 0x0f, 0x03, 0x00, 0x00};
  YCbCr420CapabilityMapDataBlock block;
  block.svd_indices = {1, 2};
  EXPECT_EQ(YCbCr420CapabilityMapDataBlock::parse_byte_block(capability_map_block.data()), block);
}

TEST(YCbCr420CapabilityMapDataBlockTests, Roundtrip) {
  YCbCr420CapabilityMapDataBlock capability_map_block;
  capability_map_block.svd_indices = {1, 2};
  auto capability_map_block_binary = capability_map_block.generate_byte_block();
  auto capability_map_block_parsed = YCbCr420CapabilityMapDataBlock::parse_byte_block(capability_map_block_binary.data());
  EXPECT_EQ(capability_map_block, capability_map_block_parsed);
}

TEST(ColorimetryDataBlockTests, Generating) {
  std::vector<uint8_t> expected = {0xe3, 0x05, 0x90, 0x51};

  ColorimetryDataBlock block{
    ENUM_NULL
    | CS_OP_RGB
    | CS_BT2020_RGB
    | CS_ICTCP
    | CS_DEFAULT_RGB,

    GMP_0
  };

  const auto actual = block.generate_byte_block();
  EXPECT_EQ(expected, actual);
}

TEST(ColorimetryDataBlockTests, Parsing) {
  std::vector<uint8_t> binary = {0xe3, 0x05, 0x51, 0x90};

  ColorimetryDataBlock expected{
    ENUM_NULL
    | CS_OP_RGB
    | CS_BT2020_YCC
    | CS_XV_YCC601
    | CS_DEFAULT_RGB
    | CS_ST2113_RGB,

    ENUM_NULL
  };

  EXPECT_EQ(ColorimetryDataBlock::parse_byte_block(binary.data()), expected);
}

TEST(ColorimetryDataBlockTests, Roundtrip) {
  ColorimetryDataBlock initial{
    ENUM_NULL
    | CS_OP_RGB
    | CS_BT2020_RGB
    | CS_ICTCP
    | CS_DEFAULT_RGB,

    GMP_0
  };

  const auto binary = initial.generate_byte_block();
  const auto parsed = ColorimetryDataBlock::parse_byte_block(binary.data());
  EXPECT_EQ(initial, parsed);
}

TEST(CommonRoundtrips, DisplaySerialNumber) {
  auto name = AsciiString{"R8J00779SL012", ASCII_SERIAL_NUMBER};
  auto name_binary = name.generate_byte_block();
  auto name_parsed = AsciiString::parse_byte_block(name_binary.begin());
  EXPECT_EQ(name, name_parsed);
}

TEST(CommonRoundtrips, EstablishedTimings3) {
  auto et_3 = EstablishedTimings3{};
  et_3.bytes_6_11[2] |= EstablishedTiming3Byte8::ET_1440x900_60;
  et_3.bytes_6_11[2] |= EstablishedTiming3Byte8::ET_1440x900_75;
  et_3.bytes_6_11[3] |= EstablishedTiming3Byte9::ET_1600x1200_60;

  const auto et_3_binary = et_3.generate_byte_block();
  const auto et_3_parsed = EstablishedTimings3::parse_byte_block(et_3_binary.begin());

  EXPECT_EQ(et_3, et_3_parsed);
}

TEST(CommonRoundtrips, DisplayName) {
  auto name = AsciiString{"TEST_NAME", ASCII_DISPLAY_NAME};
  auto name_binary = name.generate_byte_block();
  auto name_parsed = AsciiString::parse_byte_block(name_binary.begin());
  EXPECT_EQ(name, name_parsed);
}

TEST(CommonRoundtrips, BaseEdid) {
  EdidData edid;
  edid.base_block = make_edid_base();
  auto edid_binary = generate_edid_binary(edid);
  std::array<uint8_t, EDID_BLOCK_SIZE> base_edid_binary;
  std::move(edid_binary.begin(), edid_binary.begin() + EDID_BLOCK_SIZE, base_edid_binary.begin());
  EXPECT_EQ(parse_base_block(base_edid_binary).first, make_edid_base());
}

TEST(CommonRoundtrips, DetailedTimingDescriptor) {
  DetailedTimingDescriptor dtd = DetailedTimingDescriptor{
    148'500'000, 1920, 1080, 280, 45, 88, 44,
    4, 5, 1039, 584, 0, 0, DtdFeaturesBitmap{false, NO_STEREO, DigitalSeparateSync{true, true}}
  };
  auto binary = dtd.generate_byte_block();
  EXPECT_EQ(dtd, DetailedTimingDescriptor::parse_byte_block(binary.data()));
}

TEST(CommonRoundtrips, VideoDataBlock) {
  VideoDataBlock video_data_block = {
    {16, 4, 31, 19, 2, 18, 1}
  };
  std::vector<uint8_t> vdb_binary = video_data_block.generate_byte_block();
  const auto vdb_parsed = VideoDataBlock::parse_byte_block(vdb_binary.data());
  EXPECT_EQ(video_data_block.vics, vdb_parsed.vics);
  EXPECT_EQ(video_data_block, vdb_parsed);
}

TEST(CommonRoundtrips, AudioDataBlock) {
  ShortAudioDescriptor sad;
  sad.audio_format = AudioFormatCode::LPCM;
  sad.channels = AudioChannels::AC_2;
  sad.sampling_freqs |= SamplingFrequence::SF_48;
  sad.sampling_freqs |= SamplingFrequence::SF_44_1;
  sad.sampling_freqs |= SamplingFrequence::SF_32;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_16;
  AudioDataBlock audio_data_block;
  audio_data_block.sads.push_back(sad);
  std::vector<uint8_t> adb_binary = audio_data_block.generate_byte_block();
  EXPECT_EQ(audio_data_block, AudioDataBlock::parse_byte_block(adb_binary.data()));
}

TEST(CommonRoundtrips, FullEdid) {
  EdidData edid{make_edid_base(), std::vector{make_cta861_ext()}};
  auto edid_binary = generate_edid_binary(edid);
  EXPECT_EQ(edid, parse_edid_binary(edid_binary));
}

TEST(DataBlockCollection, DataBlockCollectionGenerating) {
  std::vector<uint8_t> dbc = {
    0x47, 0x10, 0x04, 0x1F, 0x13, 0x02, 0x12, 0x01, 0x23, 0x09, 0x07, 0x01, 0x67, 0xd8, 0x5d, 0xc4,
    0x01, 0x78, 0xc0, 0x00, 0x65, 0x03, 0x0c, 0x00, 0x20, 0x00
  };

  VideoDataBlock video_data_block = {
    {16, 4, 31, 19, 2, 18, 1}
  };

  ShortAudioDescriptor sad;
  sad.audio_format = AudioFormatCode::LPCM;
  sad.channels = AudioChannels::AC_2;
  sad.sampling_freqs |= SamplingFrequence::SF_48;
  sad.sampling_freqs |= SamplingFrequence::SF_44_1;
  sad.sampling_freqs |= SamplingFrequence::SF_32;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_16;

  AudioDataBlock audio_data_block;
  audio_data_block.sads.push_back(sad);

  UnknownDataBlock unknown_block_1{
    std::vector<uint8_t>{0xd8, 0x5d, 0xc4, 0x01, 0x78, 0xc0, 0x00},
    CTA861_VENDOR_DATA_BLOCK_TAG,
    std::nullopt
  };

  HdmiVendorDataBlock hdmi_vsdb{
    {2, 0, 0, 0}
  };

  DataBlockCollection data_block_collection{
    video_data_block,
    audio_data_block,
    unknown_block_1,
    hdmi_vsdb
  };

  auto dbc_binary = generate_data_block_collection(data_block_collection);
  EXPECT_EQ(dbc, dbc_binary);
}

TEST(DataBlockCollection, Roundtrip) {
  VideoDataBlock video_data_block = {
    {16, 4, 31, 19, 2, 18, 1}
  };

  ShortAudioDescriptor sad;
  sad.audio_format = AudioFormatCode::LPCM;
  sad.channels = AudioChannels::AC_2;
  sad.sampling_freqs |= SamplingFrequence::SF_48;
  sad.sampling_freqs |= SamplingFrequence::SF_44_1;
  sad.sampling_freqs |= SamplingFrequence::SF_32;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_16;

  AudioDataBlock audio_data_block;
  audio_data_block.sads.push_back(sad);

  UnknownDataBlock unknown_block_1{
    std::vector<uint8_t>{0xd8, 0x5d, 0xc4, 0x01, 0x78, 0xc0, 0x00},
    CTA861_VENDOR_DATA_BLOCK_TAG
  };

  HdmiVendorDataBlock hdmi_vsdb{
    {2, 0, 0, 0}
  };

  DataBlockCollection data_block_collection{
    video_data_block,
    audio_data_block,
    unknown_block_1,
    hdmi_vsdb
  };

  auto dbc_binary = generate_data_block_collection(data_block_collection);
  auto dbc_parsed = parse_data_block_collection(dbc_binary);

  EXPECT_EQ(data_block_collection, dbc_parsed);
}


TEST(Cta861Block, Roundtrip) {
  std::array<uint8_t, EDID_BLOCK_SIZE> cta861_binary = generate_cta861_block(make_cta861_ext());
  EXPECT_EQ(make_cta861_ext(), parse_cta861_block(cta861_binary));
}

TEST(Cta861Block, Cta861BlockGenerating) {
  std::array<uint8_t, EDID_BLOCK_SIZE> cta861_binary = {
    0x02, 0x03, 0x10, 0xF0, 0x47, 0x10, 0x04, 0x1F, 0x13, 0x02, 0x12, 0x01, 0x23, 0x09, 0x07, 0x01,
    0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0x0F, 0x48, 0x42, 0x00,
    0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB
  };
  EXPECT_EQ(cta861_binary, generate_cta861_block(make_cta861_ext()));
}

TEST(ForEachModeTests, DeleteModesFromBaseEdid) {
  BaseBlock edid_base_before = make_edid_base();
  BaseBlock edid_base_after = edid_base_before;

  remove_mode_if(edid_base_before, [](const VideoTimingMode& mode) {
    return mode.v_res == 600 || mode.v_res == 1080;
  });

  edid_base_after.established_timings_1 &= ~EstablishedTiming1::ET_800x600_56;
  edid_base_after.eighteen_byte_descriptors[0] = DummyDescriptor{};
  std::get<EstablishedTimings3>(edid_base_after.eighteen_byte_descriptors[3]).bytes_6_11[0] &= ~EstablishedTiming3Byte6::ET_800x600_85;
  edid_base_after.standard_timings[4] = std::nullopt;
  EXPECT_EQ(edid_base_before, edid_base_after);
}

TEST(ForEachModeTests, DeleteModesFromCta861) {
  Cta861Block cta861_before = make_cta861_ext();
  auto iter = std::find_if(cta861_before.data_block_collection.begin(), cta861_before.data_block_collection.end(), [](const auto& data_block){
    return std::visit(is_vdb_visitor, data_block);
  });
  *iter = VideoDataBlock{
    {16, 4, 31, 19, 2, 18, 1, 225}
  };

  remove_mode_if(cta861_before, [](const VideoTimingMode& mode) {
    return mode.v_res == 600 || mode.v_res == 1080;
  });

  Cta861Block cta861_after = make_cta861_ext();
  iter = std::find_if(cta861_after.data_block_collection.begin(), cta861_after.data_block_collection.end(), [](const auto& data_block){
    return std::visit(is_vdb_visitor, data_block);
  });
  *iter = VideoDataBlock{
    {4, 19, 2, 18, 1, 225}
  };
  cta861_after.detailed_timing_descriptors = {};

  EXPECT_EQ(cta861_before, cta861_after);
}

TEST(ForEachModeTests, DeleteModesFromHdmiVsdb) {
  Cta861Block cta861_before = make_cta861_ext();
  HdmiVendorDataBlock block_before{
    {1, 0, 0, 0}
  };
  block_before.hdmi_video = HdmiVideoSubblock{ISM_NO_INFO, {1, 2, 3, 4}};
  cta861_before.data_block_collection.push_back(block_before);

  remove_mode_if(cta861_before, [](const VideoTimingMode& mode) {
    return mode.h_res == 3840;
  });

  Cta861Block cta861_after = make_cta861_ext();
  HdmiVendorDataBlock block_after{
    {1, 0, 0, 0}
  };
  block_after.hdmi_video = HdmiVideoSubblock{ISM_NO_INFO, { 4}};
  cta861_after.data_block_collection.push_back(block_after);

  EXPECT_EQ(cta861_before, cta861_after);
}


TEST(ForEachModeTests, DeleteModesFromCta861IncludingUnknown) {
  Cta861Block cta861_before = make_cta861_ext();
  auto iter = std::find_if(cta861_before.data_block_collection.begin(), cta861_before.data_block_collection.end(), [](const auto& data_block){
    return std::visit(is_vdb_visitor, data_block);
  });
  *iter = VideoDataBlock{
    {16, 4, 31, 19, 2, 18, 1, 225}
  };

  remove_mode_if(cta861_before, [](const VideoTimingMode& mode) {
    return mode.v_res == 600 || mode.v_res == 1080;
  }, true);

  Cta861Block cta861_after = make_cta861_ext();
  iter = std::find_if(cta861_after.data_block_collection.begin(), cta861_after.data_block_collection.end(), [](const auto& data_block){
    return std::visit(is_vdb_visitor, data_block);
  });
  *iter = VideoDataBlock{
    {4, 19, 2, 18, 1}
  };
  cta861_after.detailed_timing_descriptors = {};

  EXPECT_EQ(cta861_before, cta861_after);
}

TEST(ForEachModeTests, DeleteModesFromOverallEdid) {
  EdidData edid_before{make_edid_base(), std::vector{make_cta861_ext()}};

  remove_mode_if(edid_before, [](const VideoTimingMode& mode) {
    return mode.v_res == 600 || mode.v_res == 1080;
  });

  EdidData edid_after{make_edid_base(), std::vector{make_cta861_ext()}};

  edid_after.base_block.established_timings_1 &= ~EstablishedTiming1::ET_800x600_56;

  edid_after.base_block.eighteen_byte_descriptors[0] = DummyDescriptor{};
  std::get<EstablishedTimings3>(edid_after.base_block.eighteen_byte_descriptors[3]).bytes_6_11[0] &= ~EstablishedTiming3Byte6::ET_800x600_85;

  edid_after.base_block.standard_timings[4] = std::nullopt;

  auto iter = std::find_if(edid_after.extension_blocks->at(0).data_block_collection.begin(), edid_after.extension_blocks->at(0).data_block_collection.end(), [](const auto& data_block){
    return std::visit(is_vdb_visitor, data_block);
  });
  *iter = VideoDataBlock{
    {4, 19, 2, 18, 1}
  };
  edid_after.extension_blocks->at(0).detailed_timing_descriptors = {};
  EXPECT_EQ(edid_before, edid_after);
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
  edid_base.product_code = 9216;
  edid_base.manufacture_date_or_model_year = ManufactureDate{28, 2016};
  edid_base.bits_per_color = BitDepth::BD_8;
  edid_base.video_interface = VideoInterface::VI_HDMIa;
  edid_base.h_screen_size = 52;
  edid_base.v_screen_size = 32;
  edid_base.display_type = DigitalDisplayType::DDT_RGB444_YCrCb444;
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
  edid_base.standard_timings[7] = StandardTiming{256, AspectRatio::AR_16_10, 60};

  edid_base.eighteen_byte_descriptors[0] = DetailedTimingDescriptor{
    148'500'000, 1920, 1080, 280, 45, 88, 44,
    4, 5, 477, 268, 0, 0, DtdFeaturesBitmap{false, NO_STEREO, DigitalSeparateSync{false, false}}
  };
  edid_base.eighteen_byte_descriptors[1] = AsciiString{"KALED24144F", ASCII_DISPLAY_NAME};
  edid_base.eighteen_byte_descriptors[2] = DisplayRangeLimits{
    40, 144, 160, 160, 330, VideoTimingSupport::VTS_DEFAULT_GTF
  };

  Cta861Block cta861;
  cta861.underscan = true;
  cta861.basic_audio = true;
  cta861.ycbcr_444 = true;
  cta861.ycbcr_422 = true;

  VideoDataBlock video_data_block = {
    {144, 5, 4, 3, 2, 1, 17, 18, 19, 20, 6, 7, 21, 22, 31, 32}
  };
  cta861.data_block_collection.push_back(video_data_block);

  ShortAudioDescriptor sad;
  sad.audio_format = AudioFormatCode::LPCM;
  sad.channels = AudioChannels::AC_2;
  sad.sampling_freqs |= SamplingFrequence::SF_48;
  sad.sampling_freqs |= SamplingFrequence::SF_44_1;
  sad.sampling_freqs |= SamplingFrequence::SF_32;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_16;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_20;
  sad.lpcm_bit_depths |= LpcmBitDepth::LPCM_BD_24;
  AudioDataBlock audio_data_block;
  audio_data_block.sads.push_back(sad);
  cta861.data_block_collection.push_back(audio_data_block);

  SpeakerAllocationDataBlock speaker_allocation_data_block;
  speaker_allocation_data_block.speaker_allocation |= FRONT_LEFT_AND_RIGHT;
  cta861.data_block_collection.push_back(speaker_allocation_data_block);

  UnknownDataBlock unknown_block_1{
    std::vector<uint8_t>{0xd8, 0x5d, 0xc4, 0x01, 0x78, 0xc0, 0x00},
    CTA861_VENDOR_DATA_BLOCK_TAG
  };

  HdmiVendorDataBlock hdmi_vsdb{
    {2, 0, 0, 0}
  };

  cta861.data_block_collection.push_back(unknown_block_1);
  cta861.data_block_collection.push_back(hdmi_vsdb);

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

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
