#!/bin/bash
set -x

echo 'Installing yhirose/cpp-httplib'
mkdir include
wget https://github.com/yhirose/cpp-httplib/archive/v0.5.1.tar.gz -O cpp-httplib.tar.gz
tar -xvf cpp-httplib.tar.gz
rm cpp-httplib.tar.gz
mv cpp-httplib-0.5.1 include/httplib 

echo 'installing nlohmann/json'
mkdir -p include/nlohmann
wget https://github.com/nlohmann/json/releases/download/v3.7.3/json.hpp -O include/nlohmann/json.hpp
rm json.hpp

echo 'installing jarro2783/cxxopts'
mkdir -p include/cxxopts
wget https://github.com/jarro2783/cxxopts/archive/v2.2.0.tar.gz
tar -xvf v2.2.0.tar.gz
rm v2.2.0.tar.gz
mv cxxopts-2.2.0/include/cxxopts.hpp include/cxxopts
rm -r cxxopts-2.2.0
