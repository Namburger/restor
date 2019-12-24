#!/bin/bash
# set -x

script_dir=$(dirname "$(readlink -f "$0")")
out_dir="$script_dir"/../all/base64
if [ -d "$out_dir" ]
  then
    echo "base64 installed"
    exit 1
  else
    echo "Installing base64"
fi

wget -O cpp-base64.zip https://github.com/ReneNyffenegger/cpp-base64/archive/master.zip
unzip cpp-base64.zip 

mkdir -p $out_dir
mv cpp-base64-master/base64* $out_dir
rm -r cpp-base64*
