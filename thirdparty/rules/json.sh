#!/bin/bash
# set -x

script_dir=$(dirname "$(readlink -f "$0")")
out_dir="$script_dir"/../all/nlohmann
if [ -d "$out_dir" ]
  then
    echo "json installed"
    exit 1
  else
    echo "Installing json"
fi

wget https://github.com/nlohmann/json/releases/download/v3.7.3/json.hpp
mkdir -p $out_dir

mv json.hpp $out_dir
