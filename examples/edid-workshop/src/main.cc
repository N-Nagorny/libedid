// Copyright 2023 N-Nagorny
#include <fstream>
#include <iostream>
#include <strstream>

#include "edid/edid.hh"
#include "edid/json.hh"
#include "edid/bcp00501.hh"

#include <clipp.h>
#include <nlohmann/json.hpp>

using namespace std;

Edid::EdidData read_edid_file(const string& path) {
  auto edid_binary = Edid::read_file(path);
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

void print_json(const string& path_to_edid) {
  auto edid_binary = Edid::read_file(path_to_edid);
  nlohmann::json j = Edid::parse_edid_binary(edid_binary);
  cout << j.dump(2) << endl;
}

void generate_from_json(const string& path_to_json, const string& path_to_edid) {
  // TODO(N-Nagorny): add validation against JSON Schema
  auto json = Edid::read_file(path_to_json);
  // TODO(N-Nagorny): replace it with something non-deprecated
  istrstream stream(reinterpret_cast<const char*>(json.data()), json.size());
  Edid::EdidData edid_data = nlohmann::json::parse(stream);
  vector<uint8_t> generated_edid = Edid::generate_edid_binary(edid_data);
  ofstream generated(path_to_edid, ios::out | ios::binary);
  generated.write((char *)&generated_edid[0], generated_edid.size());
}

int main(int argc, char* argv[]) {
  using namespace clipp;

  enum class mode {
    decode,
    help,
    to_json,
    from_json,
    bcp00501
  };
  mode selected = mode::decode;
  vector<string> input;

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

  auto bcp00501Mode = (
    command("bcp_005_01").set(selected, mode::bcp00501),
    value("edid_binary", input)
  );

  auto cli = (
    ( decodeMode
    | toJsonMode
    | fromJsonMode
    | bcp00501Mode
    | command("help").set(selected, mode::help)
    )
  );

  if (parse(argc, argv, cli)) {
    try {
      switch(selected) {
        case mode::decode:
          print_decoded_edid(input.at(0));
          break;
        case mode::to_json:
          print_json(input.at(0));
          break;
        case mode::from_json:
          generate_from_json(input.at(0), input.at(1));
          break;
        case mode::bcp00501: {
          nlohmann::json j = generate_constraint_sets(read_edid_file(input.at(0)));
          cout << j.dump(2) << endl;
          break;
        }
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
