#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "edgetpu.h"
#include "glog/logging.h"
#include "nlohmann/json.hpp"
#include "src/utils/server.h"
#include "yaml-cpp/yaml.h"

ABSL_FLAG(std::string, config_path, "config/restor.yaml", "Path to the config.");
ABSL_FLAG(std::string, model_path, "", "full path to the models.");
ABSL_FLAG(std::string, label_path, "", "full path to the label file.");
ABSL_FLAG(std::string, server_port, "", "serving port");
ABSL_FLAG(size_t, num_results, 0, "number of results to return on each inference run");
ABSL_FLAG(size_t, num_threads, 0, "number of threads to run server");

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

  YAML::Node conf;
  if (!access(absl::GetFlag(FLAGS_config_path).c_str(), R_OK)) {
    // conf = nlohmann::json::parse(std::ifstream(absl::GetFlag(FLAGS_config_path)));
    auto yaml_node = YAML::LoadFile(absl::GetFlag(FLAGS_config_path));
    conf = yaml_node["restor"];
  }

  // overwrite config with flags
  const auto& model_path = absl::GetFlag(FLAGS_model_path).empty()
                               ? conf["modelFile"].as<std::string>()
                               : absl::GetFlag(FLAGS_model_path);
  const auto& labels_path = absl::GetFlag(FLAGS_label_path).empty()
                                ? conf["labelFile"].as<std::string>()
                                : absl::GetFlag(FLAGS_label_path);
  const auto& server_port = absl::GetFlag(FLAGS_server_port).empty()
                                ? conf["port"].as<std::string>()
                                : absl::GetFlag(FLAGS_server_port);
  auto num_results = (absl::GetFlag(FLAGS_num_results)) ? absl::GetFlag(FLAGS_num_results)
                                                        : conf["numResults"].as<size_t>();
  auto num_threads = (absl::GetFlag(FLAGS_num_threads)) ? absl::GetFlag(FLAGS_num_threads)
                                                        : conf["numThreads"].as<size_t>();

  restor::DetectionServer s(model_path, labels_path, num_results, server_port, num_threads);

  return 0;
}
