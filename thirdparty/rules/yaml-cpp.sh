#!/bin/bash
# set -x

script_dir=$(dirname "$(readlink -f "$0")")
parent_dir="$(dirname "$script_dir")"
out_dir="$parent_dir"/all/yaml-cpp
build_dir="$script_dir"/yaml-cpp
if [ -d "$out_dir" ]
  then
    echo "yaml-cpp already installed"
    exit 1
  else
    echo "Installing yaml-cpp"
fi

# Specify the release you want here
package=yaml-cpp-0.6.3.tar.gz
wget https://github.com/jbeder/yaml-cpp/archive/"$package"

mkdir -p $out_dir
mkdir -p $build_dir 
mv "$package" $build_dir
cd $build_dir
tar -xf "$package" --strip-components=1

"$parent_dir"/all/cmake/cmake-3.16.0-Linux-x86_64/bin/cmake \
 -DBUILD_SHARED_LIBS=OFF \
 -DCMAKE_INSTALL_PREFIX="$out_dir" \
 .

make -j$(nproc)
make install

rm -rf $build_dir
