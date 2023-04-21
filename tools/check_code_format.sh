#!/bin/sh

# Check code format.
# Usage: ./tools/check_code_format.sh

find src test -name \*.cpp -or -name \*.hpp | xargs clang-format -output-replacements-xml | grep -q "<replacement " \
  && echo "Code is not formatted correctly" && exit 1 \
  || echo "Code is formatted correctly"
