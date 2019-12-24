#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "edgetpu.h"
#include "glog/logging.h"
#include "nlohmann/json.hpp"
#include "served/request_error.hpp"
#include "src/utils/server.h"

ABSL_FLAG(std::string, config_path, "config/restor.json", "Path to the config.");
ABSL_FLAG(
    std::string, model_path, "test_data/mobilenet_ssd_v2_coco_quant_postprocess_edgetpu.tflite",
    "full path to the models.");
ABSL_FLAG(std::string, label_path, "test_data/coco_labels.txt", "full path to the label file.");
ABSL_FLAG(std::string, port, "8888", "serving port");
ABSL_FLAG(size_t, num_results, 5, "number of results to return");
ABSL_FLAG(size_t, num_threads, 4, "number of threads to run server");

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);

  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;

  LOG(INFO) << "RESTOR\n";
  const auto& available_tpus = edgetpu::EdgeTpuManager::GetSingleton()->EnumerateEdgeTpu();
  if (available_tpus.size() < 1) {
    LOG(ERROR) << "No TPU found\n";
    std::abort();
  }

  LOG(INFO) << "found " << available_tpus.size() << " TPU(s)\n";

  LOG(INFO) << "config: " << absl::GetFlag(FLAGS_config_path);
  nlohmann::json conf;
  if (!access(absl::GetFlag(FLAGS_config_path).c_str(), R_OK)) {
    conf = nlohmann::json::parse(std::ifstream(absl::GetFlag(FLAGS_config_path)));
  }

  // overwrite config with flags
  const auto& model_path = absl::GetFlag(FLAGS_model_path).empty()
                               ? conf["modelFile"].get<std::string>()
                               : absl::GetFlag(FLAGS_model_path);
  const auto& labels_path = absl::GetFlag(FLAGS_label_path).empty()
                                ? conf["labelFile"].get<std::string>()
                                : absl::GetFlag(FLAGS_label_path);
  auto num_results = (absl::GetFlag(FLAGS_num_results) != conf["numResults"].get<size_t>())
                         ? absl::GetFlag(FLAGS_num_results)
                         : conf["numResults"].get<size_t>();
  auto num_threads = (absl::GetFlag(FLAGS_num_threads) != conf["numThreads"].get<size_t>())
                         ? absl::GetFlag(FLAGS_num_threads)
                         : conf["numThreads"].get<size_t>();

  const auto& port = absl::GetFlag(FLAGS_port).empty() ? conf["port"].get<std::string>()
                                                       : absl::GetFlag(FLAGS_port);

  restor::DetectionServer s(model_path, labels_path, num_results, port, num_threads);

  return 0;
}
