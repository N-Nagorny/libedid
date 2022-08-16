#include <filesystem>
#include <fstream>
#include <iostream>

#include "edid/edid.hh"

#include "clipp.h"

using namespace std;

vector<uint8_t> read_file(const string& file_path)
{
  if (!filesystem::is_regular_file(file_path))
    throw runtime_error(file_path + " is not a regular file");

  ifstream file(file_path,
    ios_base::in | ios_base::binary
  );
  file.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);

  // Stop eating new lines in binary mode!!!
  file.unsetf(ios::skipws);

  // get its size:
  streampos file_size;

  file.seekg(0, ios::end);
  file_size = file.tellg();
  file.seekg(0, ios::beg);

  // reserve capacity
  vector<uint8_t> vec(file_size);

  // read the data:
  file.read(reinterpret_cast<char*>(vec.data()), file_size);

  return vec;
}

Edid::EdidData read_edid_file(const string& path) {
  auto edid_binary = read_file(path);
  return Edid::parse_edid_binary(edid_binary);
}

void print_decoded_edid(const string& path) {
  auto edid = read_edid_file(path);
  Edid::print_base_block(std::cout, edid.base_block);
  if (edid.extension_blocks.has_value()) {
    for (const auto& ext_block : edid.extension_blocks.value())
      Edid::print_cta861_block(std::cout, ext_block);
  }
}

void circular_test(const string& path) {
  auto edid_binary = read_file(path);
  auto edid = Edid::parse_edid_binary(edid_binary);
  auto generated_edid_binary = Edid::generate_edid_binary(edid);
  bool success = edid_binary == generated_edid_binary;
  cout << path << " was" << (success ? "" : " NOT") <<
    " successfully parsed" << endl;
}

int main(int argc, char* argv[]) {
  using namespace clipp;

  enum class mode { test_run, decode, help };
  mode selected = mode::decode;
  vector<string> input;

  auto testRunMode = (
    command("test-run").set(selected, mode::test_run),
    values("edid_binary", input)
  );

  auto decodeMode = (
    command("decode").set(selected, mode::decode),
    values("edid_binary", input)
  );

  auto cli = (
    (decodeMode | testRunMode | command("help").set(selected, mode::help) )
  );

  if (parse(argc, argv, cli)) {
    try {
      switch(selected) {
        case mode::test_run:
          circular_test(input.at(0));
          break;
        case mode::decode:
          print_decoded_edid(input.at(0));
          break;
        case mode::help: cout << make_man_page(cli, "edid-workshop"); break;
      }
    }
    catch (const exception& e) {
      cout << "An exception occured while executing: " << e.what() << endl;
      return -1;
    }
  } else {
    cout << usage_lines(cli, "edid-workshop") << '\n';
  }
  return 0;
}
