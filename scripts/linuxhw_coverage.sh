#! /usr/bin/env bash

set -euox pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "${ROOT}"

# -------------------

function download_edid_db {
  curl -sOL https://github.com/linuxhw/EDID/archive/98bc7d6.zip
}

function decoded_to_binary {
  local INDEX_PATH="${1}"
  local IGNORE=(
    "Digital/Apple/APPAE22/F1F2AA8940D8"
    "Digital/Apple/APPAE22/89635EDE05B1"
    "Digital/Panasonic/MEIA09B/74CA14EAA297"
    "Digital/Goldstar/GSM7721/0BEB31F10C69"
    "Digital/Goldstar/GSM5BEE/C89AA0AB1877"
    "Digital/Samsung/SAM71AA/1400C94682E9"
    "Digital/Samsung/SAM0F99/3786C8CBE6E6"
    "Digital/Samsung/SAM0F99/BBE956437D7A"
    "Digital/Samsung/SAM0F99/2BB6DC1C7E25"
    "Digital/Samsung/SAM0F99/3AD662886597"
    "Digital/Samsung/SAM0F99/450187269188"
    "Digital/Samsung/SAM7052/1629EDDC93BD"
    "Digital/Samsung/SAM7052/0901E951EE99"
    "Digital/Others/MSF1003/2FEBAB12F824"
    "Digital/Others/MSF1003/4BD63FECAB10"
    "Digital/Others/MSF1003/03CA7D56B4D6"
    "Digital/BOE/BOE9001/33E71EDD4E0D"
    "Digital/Dell/DEL4284/317CAF62DB5C"
  )
  local BINARY_PATH="$(echo "${INDEX_PATH}" | sed 's/decoded/binary/')"

  readarray -t FILES < "${INDEX_PATH}"

  rm -f "${BINARY_PATH}"

  for file in "${FILES[@]}"; do
    local IS_IGNORED=false
    for ignore_pattern in "${IGNORE[@]}"; do
      if [[ $file =~ $ignore_pattern ]]; then
        IS_IGNORED=true
      fi
    done
    if [ "$IS_IGNORED" = "false" ]; then
      cat "$file" | grep -E '^([a-f0-9]{32}|[a-f0-9 ]{47})$' | tr -d '[:space:]' | xxd -r -p > "$file".bin
      echo "${file}.bin" >> "${BINARY_PATH}"
    fi
  done
}

function test_run {
  local BIN_INDEX_PATH="${1}"

  readarray -t FILES < "${BIN_INDEX_PATH}"

  rm -rf results/

  ../build/test-runner \
    --gtest_output=json:results/test_${BIN_INDEX_PATH}.json \
    --gtest_filter=PlaceholderName/EdidRoundtripTest.EdidRoundtrip/* \
    "${FILES[@]}" \
    > /dev/null 2>&1
}

process_test_results() {
  local TESTS=0
  local FAILURES=0

  for file in results/test_*.json; do
    TESTS=$(expr $TESTS + $(sed "2q;d" "$file" | grep -o '[0-9]\+'))
    FAILURES=$(expr $FAILURES + $(sed "3q;d" "$file" | grep -o '[0-9]\+'))

  done

  local SUCCESS=$(expr ${TESTS} - ${FAILURES})
  echo "${SUCCESS}/${TESTS}"
  echo "$(( ${SUCCESS} * 100 / ${TESTS} )) %"
}

# -------------------

if [[ ! -d "linuxhw_edid" ]]; then
  mkdir "linuxhw_edid"
  cd "linuxhw_edid"

  time download_edid_db
  time unzip -qq "98bc7d6.zip"
  rm "98bc7d6.zip"

  find "EDID-98bc7d6e2c0eaad61346a8bf877b562fee16efc3/Digital" -type "f" > index_decoded.txt
  split -l 15000 "index_decoded.txt" "index_decoded_"

  set +x

  for file in index_decoded_*; do
    decoded_to_binary "${file}" &
  done

  set -x

  time wait

  set +x

  for file in index_binary_*; do
    test_run "${file}" &
  done

  set -x

  time wait

  process_test_results
fi
