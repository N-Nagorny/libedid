// Copyright 2023 N-Nagorny
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>

#include "edid/common.hh"
#include "edid/dtd.hh"
#include "edid/eighteen_byte_descriptors.hh"
#include "edid/filesystem.hh"

namespace Edid {
  uint8_t calculate_block_checksum(const std::array<uint8_t, EDID_BLOCK_SIZE>& block) {
    int sum = 0;
    for (int i = 0; i < EDID_BLOCK_SIZE - 1; ++i)
      sum += block.at(i);
    return 256 - sum % 256;
  }

  std::vector<uint8_t> read_file(const std::string& file_path) {
    using namespace std;

    if (!filesystem::is_regular_file(file_path))
      throw runtime_error(file_path + " is not a regular file");

    ifstream file(file_path,
      ios_base::in | ios_base::binary
    );
    file.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);

    // Stop eating new lines in binary mode!!!
    file.unsetf(ios::skipws);

    // get its size
    streampos file_size;

    file.seekg(0, ios::end);
    file_size = file.tellg();
    file.seekg(0, ios::beg);

    // reserve capacity
    vector<uint8_t> vec(file_size);

    // read the data
    file.read(reinterpret_cast<char*>(vec.data()), file_size);

    return vec;
  }
}  // namespace Edid
