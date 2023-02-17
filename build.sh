#!/usr/bin/env bash

pushd dependencies &> /dev/null

if [[ ! -f ./cmake-3.26.0-rc3-linux-x86_64/bin/cmake ]]; then
    tar -xf ./cmake-3.26.0-rc3-linux-x86_64.tar.gz
fi

PATH="$PATH:$(pwd)/cmake-3.26.0-rc3-linux-x86_64/bin"

popd &> /dev/null

mkdir -p build

cd build

echo "Configuring project"
cmake -S .. -B .

echo "Building decaf-22"
make
