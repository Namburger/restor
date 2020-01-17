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
workspace(name = "restor")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

http_archive(
    name = "io_bazel_rules_closure",
    sha256 = "5b00383d08dd71f28503736db0500b6fb4dda47489ff5fc6bed42557c07c6ba9",
    strip_prefix = "rules_closure-308b05b2419edb5c8ee0471b67a40403df940149",
    urls = [
        "https://storage.googleapis.com/mirror.tensorflow.org/github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",
        "https://github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",  # 2019-06-13
    ],
)

http_archive(
    name = "bazel_skylib",
    sha256 = "1dde365491125a3db70731e25658dfdd3bc5dbdfd11b840b3e987ecf043c7ca0",
    urls = ["https://github.com/bazelbuild/bazel-skylib/releases/download/0.9.0/bazel_skylib-0.9.0.tar.gz"],
)

http_archive(
    name = "com_google_glog",
    # For security purpose, can use `sha256sum` on linux to calculate.
    sha256 = "835888ec47ee8065b3098f3ec4373717d641954970f009833ed6d466c397409a",
    strip_prefix = "glog-41f4bf9cbc3e8995d628b459f6a239df43c2b84a",
    urls = [
        "https://github.com/google/glog/archive/41f4bf9cbc3e8995d628b459f6a239df43c2b84a.tar.gz",
    ],
    build_file_content = """
licenses(['notice'])
load(':bazel/glog.bzl', 'glog_library')
glog_library(with_gflags=0)
""",
)

TENSORFLOW_COMMIT = "5d0b55dd4a00c74809e5b32217070a26ac6ef823"
# One way to find this SHA256 number is to download the corresponding tar.gz file
# and then run `sha256sum` on local machine.
TENSORFLOW_SHA256 = "2602e177164e7152bd8c7a9a1cab71898ec84ae707ca442cafbd966abdbb07b7"

http_archive(
    name = "org_tensorflow",
    sha256 = TENSORFLOW_SHA256,
    strip_prefix = "tensorflow-" + TENSORFLOW_COMMIT,
    urls = [
        "https://github.com/tensorflow/tensorflow/archive/" + TENSORFLOW_COMMIT + ".tar.gz",
    ],
)

load("@org_tensorflow//tensorflow:workspace.bzl", "tf_workspace")
tf_workspace(tf_repo_name = "org_tensorflow")

http_archive(
  name = "com_github_google_benchmark",
  sha256 = "59f918c8ccd4d74b6ac43484467b500f1d64b40cc1010daa055375b322a43ba3",
  strip_prefix = "benchmark-16703ff83c1ae6d53e5155df3bb3ab0bc96083be",
  urls = [
    "https://github.com/google/benchmark/archive/16703ff83c1ae6d53e5155df3bb3ab0bc96083be.zip"
  ],
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")
http_archive(
    name = "com_github_jupp0r_prometheus_cpp",
    strip_prefix = "prometheus-cpp-master",
    urls = ["https://github.com/jupp0r/prometheus-cpp/archive/master.zip"],
)

load("@com_github_jupp0r_prometheus_cpp//bazel:repositories.bzl", "prometheus_cpp_repositories")
prometheus_cpp_repositories()

new_local_repository(
    name = "libedgetpu",
    path = "thirdparty/libedgetpu",
    build_file = "thirdparty/libedgetpu/BUILD"
)

http_archive(
    name = "coral_crosstool",
    sha256 = "cb31b1417ccdcf7dd9fca5ec63e1571672372c30427730255997a547569d2feb",
    strip_prefix = "crosstool-9e00d5be43bf001f883b5700f5d04882fea00229",
    urls = [
        "https://github.com/google-coral/crosstool/archive/9e00d5be43bf001f883b5700f5d04882fea00229.tar.gz",
    ],
)
load("@coral_crosstool//:configure.bzl", "cc_crosstool")
cc_crosstool(name = "crosstool")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
  name = "com_google_absl",
  remote = "https://github.com/abseil/abseil-cpp.git",
  commit = "29235139149790f5afc430c11cec8f1eb1677607",
)

git_repository(
  name = "com_github_meltwater_served",
  remote = "https://github.com/meltwater/served.git",
  commit = "7e3f3509ec8c235ff8f68f1b022404fc1e4b977b",
)

git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "552baa34c17984ca873cd0c4fdd010c177737b6f",
    remote = "https://github.com/nelhage/rules_boost",
)
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

git_repository(
    name = "com_jbeder_yaml_cpp",
    remote = "https://github.com/jbeder/yaml-cpp.git",
    commit = "62ff351432ebde809585bc2e48305bd5349e4b84",
)


