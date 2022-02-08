{ }:

with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "libedid";
  buildInputs = [ gtest ];
  nativeBuildInputs = [ cmake pkg-config ];

  src = ./.;
}
