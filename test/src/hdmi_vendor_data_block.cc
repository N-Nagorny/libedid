#include <gtest/gtest.h>

#include "edid/hdmi_vendor_data_block.hh"

using namespace Edid;

TEST(HdmiVdb, Generating) {
  HdmiVendorDataBlock hdmi_vdb;
  hdmi_vdb.source_phy_addr = {1, 0, 0, 0};
  hdmi_vdb.capabilities = HVDBB6F_SUPPORTS_AI
    | HVDBB6F_DC_36BIT
    | HVDBB6F_DC_30BIT
    | HVDBB6F_DC_Y444;
  hdmi_vdb.max_tmds_clock_mhz = 340;
  HdmiVideoSubblock subblock;
  subblock.hdmi_vics = {1, 2, 3, 4};
  StereoVideoSupport svs;
  svs.formats = {SVF_SIDE_BY_SIDE_HORIZONTAL, SVF_TOP_AND_BOTTOM | SVF_FRAME_PACKING};
  svs.vics = 0;
  subblock.stereo_video_support = svs;
  hdmi_vdb.hdmi_video = subblock;

  std::vector<uint8_t> hdmi_vdb_binary{
    0x72, 3, 12, 0, 16, 0, 184, 68, 32, 192, 132,
    1, 2, 3, 4, 1, 65, 0, 0
  };

  EXPECT_EQ(hdmi_vdb.generate_byte_block(), hdmi_vdb_binary);
}


TEST(HdmiVdb, CircularTest) {
  HdmiVendorDataBlock hdmi_vdb;
  hdmi_vdb.source_phy_addr = {1, 0, 0, 0};
  hdmi_vdb.capabilities = HVDBB6F_SUPPORTS_AI
    | HVDBB6F_DC_36BIT
    | HVDBB6F_DC_30BIT
    | HVDBB6F_DC_Y444;
  hdmi_vdb.max_tmds_clock_mhz = 340;
  HdmiVideoSubblock subblock;
  subblock.hdmi_vics = {1, 2, 3, 4};
  StereoVideoSupport svs;
  svs.formats = {SVF_SIDE_BY_SIDE_HORIZONTAL, SVF_TOP_AND_BOTTOM | SVF_FRAME_PACKING};
  svs.vics = 0;
  subblock.stereo_video_support = svs;
  hdmi_vdb.hdmi_video = subblock;

  std::vector<uint8_t> hdmi_vdb_binary = hdmi_vdb.generate_byte_block();

  const auto parsed = HdmiVendorDataBlock::parse_byte_block(hdmi_vdb_binary.begin());
  EXPECT_EQ(parsed.source_phy_addr, hdmi_vdb.source_phy_addr);
  EXPECT_EQ(parsed.capabilities, hdmi_vdb.capabilities);
  EXPECT_EQ(parsed.max_tmds_clock_mhz, hdmi_vdb.max_tmds_clock_mhz);
  EXPECT_EQ(parsed.content_types, hdmi_vdb.content_types);
  EXPECT_EQ(parsed.latency, hdmi_vdb.latency);
  EXPECT_EQ(parsed.interlaced_latency, hdmi_vdb.interlaced_latency);
  EXPECT_EQ(parsed.hdmi_video->image_size_meaning, hdmi_vdb.hdmi_video->image_size_meaning);
  EXPECT_EQ(parsed.hdmi_video->hdmi_vics, hdmi_vdb.hdmi_video->hdmi_vics);
  EXPECT_EQ(parsed.hdmi_video->stereo_video_support->formats, hdmi_vdb.hdmi_video->stereo_video_support->formats);
  EXPECT_EQ(parsed.hdmi_video->stereo_video_support->vics, hdmi_vdb.hdmi_video->stereo_video_support->vics);
  EXPECT_EQ(parsed.hdmi_video->vic_3d_support, hdmi_vdb.hdmi_video->vic_3d_support);
  EXPECT_EQ(parsed, hdmi_vdb);
}

