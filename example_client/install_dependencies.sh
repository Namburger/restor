#!/bin/bash
set -x

echo 'Installing yhirose/cpp-httplib'
wget https://github.com/yhirose/cpp-httplib/archive/v0.5.1.tar.gz -O cpp-httplib.tar.gz
mkdir include
tar -xvf cpp-httplib.tar.gz
rm cpp-httplib.tar.gz
mv cpp-httplib-0.5.1 include/httplib 

echo 'Installing nlohmann/json'
mkdir -p include/nlohmann
wget https://github.com/nlohmann/json/releases/download/v3.7.3/json.hpp -O include/nlohmann/json.hpp
