#include <filesystem>
#include <fstream>
#include <iostream>
#include <strstream>

#include "edid/edid.hh"
#include "edid/json.hh"

#include <clipp.h>
#include <nlohmann/json.hpp>

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
    " successfully parsed and generated back" << endl;
}

void print_json(const string& path_to_edid) {
  auto edid_binary = read_file(path_to_edid);
  nlohmann::json j = Edid::parse_edid_binary(edid_binary);
  cout << j.dump(2) << endl;
}

void generate_from_json(const string& path_to_json, const string& path_to_edid) {
  // TODO: add validation against JSON Schema
  auto json = read_file(path_to_json);
  // TODO: replace it with something non-deprecated
  istrstream stream(reinterpret_cast<const char*>(json.data()), json.size());
  Edid::EdidData edid_data = nlohmann::json::parse(stream);
  vector<uint8_t> generated_edid = Edid::generate_edid_binary(edid_data);
  ofstream generated(path_to_edid, ios::out | ios::binary);
  generated.write((char *)&generated_edid[0], generated_edid.size());
}

int main(int argc, char* argv[]) {
  using namespace clipp;

  enum class mode {
    test_run,
    decode,
    help,
    to_json,
    from_json
  };
  mode selected = mode::decode;
  vector<string> input;

  auto testRunMode = (
    command("test_run").set(selected, mode::test_run),
    value("edid_binary", input)
  );

  auto decodeMode = (
    command("decode").set(selected, mode::decode),
    value("edid_binary", input)
  );

  auto toJsonMode = (
    command("to_json").set(selected, mode::to_json),
    value("edid_binary", input)
  );

  auto fromJsonMode = (
    command("from_json").set(selected, mode::from_json),
    value("json", input),
    value("edid_binary", input)
  );

  auto cli = (
    ( decodeMode
    | testRunMode
    | toJsonMode
    | fromJsonMode
    | command("help").set(selected, mode::help)
    )
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
        case mode::to_json:
          print_json(input.at(0));
          break;
        case mode::from_json:
          generate_from_json(input.at(0), input.at(1));
          break;
        default:
          cout << make_man_page(cli, "edid-workshop");
          break;
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
