# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
SHELL := /bin/bash
MAKEFILE_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(MAKEFILE_DIR)/src
BAZEL := $(MAKEFILE_DIR)/thirdparty/all/bazel/bazel

SRCS := $(shell find $(SRC_DIR) -name *.cc -or -name *.h)
$(info Formatting [${SRCS}])
$(shell thirdparty/all/clang/clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04/bin/clang-format -i $(SRCS))

# Allowed CPU values: k8, armv7a, aarch64
CPU ?= k8
# Allowed COMPILATION_MODE values: opt, dbg
COMPILATION_MODE ?= opt

BAZEL_OUT_DIR :=  $(MAKEFILE_DIR)/bazel-out/$(CPU)-$(COMPILATION_MODE)/bin
BAZEL_BUILD_FLAGS := --crosstool_top=@crosstool//:toolchains \
                     --compilation_mode=$(COMPILATION_MODE) \
                     --verbose_failures \
                     --compiler=gcc \
                     --cpu=$(CPU) \
                     --sandbox_debug \
                     --linkopt=-L$(MAKEFILE_DIR)/libedgetpu/direct/$(CPU) \
                     --linkopt=-l:libedgetpu.so.1

OUT_DIR := $(MAKEFILE_DIR)/out/$(CPU)

.PHONY: all \
	main \
	tests

all: tests main

main:
	$(BAZEL) build $(BAZEL_BUILD_FLAGS) //src:restor
	mkdir -p $(OUT_DIR)
	cp -f $(MAKEFILE_DIR)/bazel-bin/src/restor $(OUT_DIR)

tests:
	$(BAZEL) build $(BAZEL_BUILD_FLAGS) $(shell bazel query 'kind(cc_.*test, //src/...)')
	mkdir -p $(OUT_DIR)/tests
	rsync -am --include='*test' --include='*/' --exclude='*' $(MAKEFILE_DIR)/bazel-bin/src $(OUT_DIR)/tests

clean:
	$(BAZEL) clean
	rm -rf ./out
