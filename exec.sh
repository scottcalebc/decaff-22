#!/usr/bin/env bash

if [[ $# -lt 1 ]]; then
    echo $#
    echo "Usage: $0 <source_file> > <output_file>"
    echo "  Token output directed to stdout"
    echo "  Must use redirection to capture"

    exit 1
fi

./build/bin/decaf-22 $1