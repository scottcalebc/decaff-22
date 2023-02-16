#!/usr/bin/bash

pushd dependencies 2> /dev/null

if [[ ! -f ./cmake-3.26.0-rc2/bin/cmake ]]; then
    tar -xvf ./cmake-3.26.0-rc2.tar.gz

    cd cmake-3.26.0-rc2

    echo "Building dependencies"
    ./bootstrap
    make

fi

PATH="$PATH:$(pwd)/cmake-3.26.0-rc2/bin"

popd 2> /dev/null

mkdir build

cd build

echo "Configuring project"
cmake -S .. -B .

echo "Building decaf-22"
make