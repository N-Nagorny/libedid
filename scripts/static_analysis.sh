#! /usr/bin/env bash

set -euox pipefail

ROOT="$(git rev-parse --show-toplevel)"
cd "${ROOT}"

# -------------------

function join_by {
  local d=${1-} f=${2-}
  if shift 2; then
    printf %s "$f" "${@/#/$d}"
  fi
}

run_cppcheck() {
  local FILES="${1}"

  cppcheck \
    "-j$(nproc)" \
    --error-exitcode=1 \
    --enable=all \
    --inline-suppr \
    --std=c++17 \
    --language=c++ \
    --force \
    ${FILES}
}

run_cpplint() {
  local FILES="${1}"
  local ROOT="${2}"
  local FILTERS=(
    "-whitespace/line_length"         # A matter of preference
    "-build/namespaces"               # To consider
    "-build/include_order"            # TODO
    "-whitespace/parens"              # A matter of preference
    "-whitespace/newline"             # A matter of preference to put a newline before "else"
    "-readability/braces"             # TODO
    "-runtime/references"             # Need to exclude "json.cc" and enable it
    "-readability/casting"            # TODO
    "-runtime/indentation_namespace"  # Not critical while a few of namespaces
  )

  cpplint \
    --root="${ROOT}" \
    --exclude=build/* \
    --exclude=result/* \
    --filter=$(join_by , ${FILTERS[@]}) \
    ${FILES}
}

# -------------------

FILES_CC="$(find . -name \*.cc | xargs echo)"
FILES_CC_HH="$(find . -name \*.hh -o -name \*.cc | xargs echo)"

run_cpplint "${FILES_CC_HH}" "${ROOT}"
run_cppcheck "${FILES_CC}"
