#!/bin/bash
# set -x

script_dir=$(dirname "$(readlink -f "$0")")
out_dir="$script_dir"/../install/bazel
if [ -d "$out_dir" ]
  then
    echo "bazel already installed"
    exit 1
  else
    echo "Installing bazel"
fi

wget https://github.com/bazelbuild/bazel/releases/download/2.0.0/bazel-2.0.0-linux-x86_64
mkdir -p $out_dir

chmod +x bazel-2.0.0-linux-x86_64
mv bazel-2.0.0-linux-x86_64 "$out_dir"/bazel
