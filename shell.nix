{ }:

with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "backoffice";
  buildInputs = [ gnumake gtest ];
}
