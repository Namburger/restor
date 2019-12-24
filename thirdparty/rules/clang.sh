#!/bin/bash
# set -x

script_dir=$(dirname "$(readlink -f "$0")")
out_dir="$script_dir"/../all/clang
if [ -d "$out_dir" ]
  then
    echo "clang already installed"
    exit 1
  else
    echo "Installing clang"
fi

wget https://releases.llvm.org/9.0.0/clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
mkdir -p $out_dir

tar -C "$out_dir" -xvf clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
rm clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz
