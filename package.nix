{
  stdenv,
  fetchFromGitHub,
  clipp,
  cmake,
  gtest,
  nlohmann_json,
  pkg-config,
}:

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
  nativeBuildInputs = [
    nlohmann_json
    cmake
  ];
};
in
stdenv.mkDerivation {
  name = "libedid";
  buildInputs = [ gtest ];
  nativeBuildInputs = [
    clipp
    cmake
    json_schema_validator
    nlohmann_json
    pkg-config
  ];
  cmakeFlags = [
    "-DENABLE_JSON=ON"
    "-DBUILD_TESTS=ON"
    "-DBUILD_EXAMPLES=ON"
  ];

  src = builtins.path { path = ./.; name = "libedid-src"; };
}
