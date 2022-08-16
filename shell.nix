{ }:

with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "libedid";
  buildInputs = [ clipp gtest nlohmann_json ];
  nativeBuildInputs = [ cmake pkg-config ];

  src = ./.;
}
