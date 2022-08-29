{ }:

with import <nixpkgs> {};

let
json_schema_validator = stdenv.mkDerivation rec {
  pname = "json_schema_validator";
  version = "2.1.0";
  src = fetchFromGitHub {
    owner = "pboettch";
    repo = "json-schema-validator";
    rev = "${version}";
    sha256 = "13d62nyfhz6kcp72dlpny36kk75zw2snf04q612bi5f5riza77xk";
  };
  buildInputs = [ nlohmann_json ];
  nativeBuildInputs = [ cmake ];
};
in
stdenv.mkDerivation {
  name = "libedid";
  buildInputs = [ clipp gtest nlohmann_json json_schema_validator ];
  nativeBuildInputs = [ cmake pkg-config ];

  src = ./.;
}
