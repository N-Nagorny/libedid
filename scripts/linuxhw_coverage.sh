# no shebang because of
# https://github.com/NixOS/nixpkgs/issues/6227

set -euox pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "${ROOT}"

# -------------------

function test_run {
  local TEST_RUNNER="${1}"
  local BIN_INDEX_PATH="${2}"
  local RESULTS_PATH="${3}"

  readarray -t FILES < "${BIN_INDEX_PATH}"

  rm -rf ${RESULTS_PATH}

  ${TEST_RUNNER} \
    --gtest_output=json:${RESULTS_PATH}/test_${BIN_INDEX_PATH}.json \
    --gtest_filter=PlaceholderName/EdidRoundtripTest.EdidRoundtrip/* \
    --gtest_brief=1 \
    "${FILES[@]}" \
    > /dev/null 2>&1
}

process_test_results() {
  local RESULTS_PATH=${1}
  local EXPECTED_SUCCESS=${2:--1}
  local TESTS=0
  local FAILURES=0

  for file in ${RESULTS_PATH}/test_*.json; do
    TESTS=$(expr $TESTS + $(sed "2q;d" "$file" | grep -o '[0-9]\+'))
    FAILURES=$(expr $FAILURES + $(sed "3q;d" "$file" | grep -o '[0-9]\+'))

  done

  local SUCCESS=$(expr ${TESTS} - ${FAILURES})
  echo "${SUCCESS}/${TESTS}"
  echo "$(( ${SUCCESS} * 100 / ${TESTS} )) %"

  if [[ ${EXPECTED_SUCCESS} -gt -1 ]]; then
    if [[ ${SUCCESS} -lt ${EXPECTED_SUCCESS} ]]; then
      exit 1
    fi
  fi
}

# -------------------

TEST_RUNNER_PATH=${1:-"./build/roundtrip-tester"}
EXPECTED_SUCCESS=${2:--1}
LINUXHW_EDID_PATH=${3:-"linuxhw_edid"}
OUTPUT_PATH=${4:-"test_results"}

TEST_RUNNER_PATH=$(realpath ${TEST_RUNNER_PATH})
LINUXHW_EDID_PATH=$(realpath ${LINUXHW_EDID_PATH})
OUTPUT_PATH=$(realpath ${OUTPUT_PATH})

rm -rf "${OUTPUT_PATH}"
mkdir "${OUTPUT_PATH}"

cd ${LINUXHW_EDID_PATH}

set +x

for file in index_binary_*; do
  test_run "${TEST_RUNNER_PATH}" "${file}" ${OUTPUT_PATH} &
done

set -x

time wait

process_test_results ${OUTPUT_PATH} ${EXPECTED_SUCCESS}
