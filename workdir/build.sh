#!/usr/bin/env bash

BASE_DIR="$(readlink -f $(dirname $0)/..)"
# push base dir so we know where we are at
pushd $BASE_DIR &> /dev/null

pushd dependencies &> /dev/null


if [[ ! -x "$(command -v cmake)" && ! -f ./cmake-3.26.1-linux-x86_64/bin/cmake ]]; then
    echo "Unpacking dependencies for build"
    tar -xf ./cmake-3.26.1-linux-x86_64.tar.gz
fi

PATH="$PATH:$(pwd)/cmake-3.26.1-linux-x86_64/bin"

popd &> /dev/null

mkdir -p build

cd build



echo "Configuring project"
cmake -S .. -B . 2> /dev/null

if [[ $? -eq 0 ]]; then
    echo "Building decaf-22"
    make install
else
    echo "Error running cmake configure..."
    echo "Running manual build"

    echo "Cleaning build directory"
    rm -rf ./build/

    mkdir -p ./build/bin

    for file in $(find ./src -iname "*.cpp" -or "*.hpp"); do
        cp $file ./build
    done

    echo "Running g++ build"
    g++ --std=c++11 build/*.cpp -o ./decaf-22 

    cp ./decaf-22 ./workdir/
fi

popd &> /dev/null
