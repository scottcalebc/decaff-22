#!/usr/bin/env bash

# Find base/source dir so we know where program exists
BASE_DIR="$(readlink -f $(dirname $0)/)"

# we don't push into source directory since file path parameter may be relative

if [[ $# -lt 1 ]]; then
    echo $#
    echo "Usage: $0 <source_file> > <output_file>"
    echo "  Token output directed to stdout"
    echo "  Must use redirection to capture"

    exit 1
fi

${BASE_DIR}/decaf-22 $1