{ }:

with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "libedid";
  buildInputs = [ gtest nlohmann_json ];
  nativeBuildInputs = [ cmake pkg-config ];

  src = ./.;
}
