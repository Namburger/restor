package(
    default_visibility = ["//visibility:public"],
)

licenses(["notice"])  # Apache 2.0

cc_library(
    name = "basic_engine_native",
    srcs = [
        "basic_engine_native.cc",
    ],
    hdrs = [
        "basic_engine_native.h",
    ],
    deps = [
        ":edgetpu_resource_manager",
        "//src/utils:error_reporter",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@libedgetpu//:header",
        "@org_tensorflow//tensorflow/lite:builtin_op_data",
        "@org_tensorflow//tensorflow/lite:framework",
        "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    ],
)

cc_library(
    name = "basic_engine",
    srcs = [
        "basic_engine.cc",
    ],
    hdrs = [
        "basic_engine.h",
    ],
    deps = [
        ":basic_engine_native",
        "@com_google_glog//:glog",
    ],
)

cc_test(
    name = "basic_engine_native_test",
    timeout = "long",
    srcs = [
        "basic_engine_native_test.cc",
    ],
    data = [
        "//test_data:images",
        "//test_data:models",
        "//test_data/invalid_models:models",
    ],
    linkstatic = 1,
    deps = [
        ":basic_engine_native",
        "//src/utils:fake_op",
        "//src/utils:test_utils",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/memory",
        "@com_google_glog//:glog",
        "@com_google_googletest//:gtest",
    ],
)

cc_test(
    name = "basic_engine_test",
    timeout = "long",
    srcs = [
        "basic_engine_test.cc",
    ],
    data = [
        "//test_data:images",
        "//test_data:models",
        "//test_data/invalid_models:models",
    ],
    linkstatic = 1,
    deps = [
        ":basic_engine",
        "//src/utils:fake_op",
        "//src/utils:test_utils",
        "@com_google_absl//absl/flags:parse",
        "@com_google_glog//:glog",
        "@com_google_googletest//:gtest",
    ],
)

cc_library(
    name = "edgetpu_resource_manager",
    srcs = [
        "edgetpu_resource_manager.cc",
    ],
    hdrs = [
        "edgetpu_resource_manager.h",
    ],
    deps = [
        "//src/utils:error_reporter",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_glog//:glog",
        "@libedgetpu//:header",
    ],
)

cc_test(
    name = "edgetpu_resource_manager_test",
    srcs = [
        "edgetpu_resource_manager_test.cc",
    ],
    linkstatic = 1,
    deps = [
        ":edgetpu_resource_manager",
        "@com_google_absl//absl/flags:parse",
        "@com_google_glog//:glog",
        "@com_google_googletest//:gtest",
    ],
)
