package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])  # Apache 2.0

cc_library(
    name = "model_utils",
    srcs = ["model_utils.cc"],
    hdrs = ["model_utils.h"],
    deps = [
        "@com_google_absl//absl/strings",
        "@libedgetpu//:header",
        "@org_tensorflow//tensorflow/lite:builtin_op_data",
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    ],
)

cc_library(
    name = "label_utils",
    srcs = ["label_utils.cc"],
    hdrs = ["label_utils.h"],
    deps = [
        "@com_google_absl//absl/strings",
    ],
) 

cc_library(
    name = "error_reporter",
    srcs = [
        "error_reporter.cc",
    ],
    hdrs = [
        "error_reporter.h",
    ],
    deps = [
        "@org_tensorflow//tensorflow/lite/core/api",
    ],
)

cc_test(
    name = "error_reporter_test",
    srcs = [
        "error_reporter_test.cc",
    ],
    linkstatic = 1,
    deps = [
        ":error_reporter",
        "@com_google_absl//absl/memory",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "version",
    srcs = [
        "version.cc",
    ],
    hdrs = [
        "version.h",
    ],
    deps = [
        "@com_google_absl//absl/base:core_headers",
        "@libedgetpu//:header",
    ],
)

cc_test(
    name = "version_test",
    srcs = [
        "version_test.cc",
    ],
    linkstatic = 1,
    deps = [
        ":version",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "utils",
    srcs = ["utils.cc"],
    hdrs = [
        "utils.h",
    ],
    deps = [
        "//src/utils:error_reporter",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
    ],
)

cc_library(
    name = "bbox_utils",
    hdrs = [
        "bbox_utils.h",
    ],
    deps = [
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "bbox_utils_test",
    srcs = [
        "bbox_utils_test.cc",
    ],
    linkstatic = 1,
    deps = [
        ":bbox_utils",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "test_utils",
    testonly = 1,
    srcs = [
        "test_utils.cc",
    ],
    hdrs = [
        "test_utils.h",
    ],
    deps = [
        ":bbox_utils",
        ":utils",
        "//src/basic:basic_engine",
        "//src/detection:engine",
        "@com_github_google_benchmark//:benchmark",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_google_googletest//:gtest",
        "@org_tensorflow//tensorflow/lite:builtin_op_data",
    ],
)

cc_library(
    name = "fake_op",
    testonly = 1,
    srcs = [
        "fake_op.cc",
    ],
    hdrs = [
        "fake_op.h",
    ],
    deps = [
        "@flatbuffers",
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite/kernels:kernel_util",
    ],
)

cc_library(
    name = "image_utils",
    srcs = [
        "image_utils.cc",
    ],
    hdrs = [
        "image_utils.h",
    ],
    deps = [
        ":bbox_utils",
        ":utils",
        "//src/basic:basic_engine",
        "//src/detection:engine",
        "//thirdparty:base64",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_glog//:glog",
        "@com_google_googletest//:gtest",
        "@org_tensorflow//tensorflow/lite:builtin_op_data",
    ],
)

cc_library(
    name = "detection_server",
    srcs = ["server.cc"],
    hdrs = ["server.h"],
    deps = [
      ":version",
      "@libedgetpu//:header",
      "@com_google_glog//:glog",
      "@com_meltwater_served//:served",
      "@org_tensorflow//tensorflow/lite:framework",
      "//thirdparty:json",
      "//src/utils:image_utils",
      "//src/utils:label_utils",
    ],
)
