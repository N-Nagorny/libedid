#! /usr/bin/env bash

set -euox pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "${ROOT}"

# -------------------

function test_run {
  local BIN_INDEX_PATH="${1}"
  local RESULTS_PATH="${2}"

  readarray -t FILES < "${BIN_INDEX_PATH}"

  rm -rf results/

  set +x

  ${ROOT}/result/bin/test-runner \
    --gtest_output=json:${RESULTS_PATH}/test_${BIN_INDEX_PATH}.json \
    --gtest_filter=PlaceholderName/EdidRoundtripTest.EdidRoundtrip/* \
    --gtest_brief=1 \
    "${FILES[@]}" \
    # > /dev/null 2>&1
}

process_test_results() {
  local RESULTS_PATH=${1}
  local TESTS=0
  local FAILURES=0

  for file in ${RESULTS_PATH}/test_*.json; do
    TESTS=$(expr $TESTS + $(sed "2q;d" "$file" | grep -o '[0-9]\+'))
    FAILURES=$(expr $FAILURES + $(sed "3q;d" "$file" | grep -o '[0-9]\+'))

  done

  local SUCCESS=$(expr ${TESTS} - ${FAILURES})
  echo "${SUCCESS}/${TESTS}"
  echo "$(( ${SUCCESS} * 100 / ${TESTS} )) %"
}

# -------------------

LINUXHW_EDID_PATH=${1:-"linuxhw_edid"}
OUTPUT_PATH=${2:-"test_results"}

LINUXHW_EDID_PATH=$(realpath ${LINUXHW_EDID_PATH})
OUTPUT_PATH=$(realpath ${OUTPUT_PATH})

rm -rf "${OUTPUT_PATH}"
mkdir "${OUTPUT_PATH}"

cd ${LINUXHW_EDID_PATH}


for file in index_binary_*; do
  test_run "${file}" ${OUTPUT_PATH} || true #&
done

set -x

time wait

process_test_results ${OUTPUT_PATH}
