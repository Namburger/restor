package(default_visibility = ["//visibility:public"])

cc_library(
    name = "json",
    hdrs = ["all/nlohmann/json.hpp"],
    includes = ["all/"],
)

cc_library(
    name = "base64",
    hdrs = ["all/base64/base64.h"],
    srcs = ["all/base64/base64.cpp"],
    includes = ["all/"],
)

cc_library(
    name = "yamlcpp",
    hdrs = glob(["all/yaml-cpp/include/yaml-cpp/*.h",
               "all/yaml-cpp/include/yaml-cpp/node/*.h",
               "all/yaml-cpp/include/yaml-cpp/node/detail/*.h",
          ]
    ),
    includes = ["all/yaml-cpp/include"],
)
