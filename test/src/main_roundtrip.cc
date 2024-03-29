// Copyright 2023 N-Nagorny

#include "edid/edid.hh"

#include <fstream>
#include <iostream>
#include <optional>

#include <gtest/gtest.h>

using namespace Edid;

static std::vector<std::string> edid_files = {};

class EdidRoundtripTest : public testing::TestWithParam<std::string> {};

TEST_P(EdidRoundtripTest, EdidRoundtrip) {
  // EDID binary -> Edid::EdidData -> EDID binary
  const auto file_path = GetParam();
  const std::vector<uint8_t> edid_binary = read_file(file_path);

  const EdidData edid = Edid::parse_edid_binary(edid_binary);
  const auto generated_edid_binary = Edid::generate_edid_binary(edid);

  EXPECT_EQ(edid_binary, generated_edid_binary);
}

INSTANTIATE_TEST_CASE_P(
  PlaceholderName,
  EdidRoundtripTest,
  testing::ValuesIn(edid_files));

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(EdidRoundtripTest);


void save_non_gtest_argv(int argc, char* argv[]) {
  const char* gtest_prefixes[] = {
    "--" GTEST_FLAG_PREFIX_,
    "--" GTEST_FLAG_PREFIX_DASH_,
    "--" GTEST_FLAG_PREFIX_UPPER_
  };

  for (int i = 1; i < argc; ++i) {
    bool save_arg = true;
    for (const char* prefix : gtest_prefixes) {
      if (strncmp(argv[i], prefix, strlen(prefix)) == 0) {
        save_arg = false;
        break;
      };
    }
    if (save_arg) {
      edid_files.push_back(argv[i]);
    }
  }
}

int main(int argc, char* argv[]) {
  save_non_gtest_argv(argc, argv);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
