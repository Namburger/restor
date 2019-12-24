#!/bin/bash
# set -x

script_dir=$(dirname "$(readlink -f "$0")")
out_dir="$script_dir"/../all/cmake
if [ -d "$out_dir" ]
  then
    echo "cmake installed"
    exit 1
  else
    echo "Installing cmake"
fi

wget https://cmake.org/files/v3.16/cmake-3.16.0-Linux-x86_64.tar.gz
mkdir -p $out_dir

tar -C "$out_dir" -xf cmake-3.16.0-Linux-x86_64.tar.gz && rm cmake-3.16.0-Linux-x86_64.tar.gz
