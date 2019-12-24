#include "src/utils/server.h"

#include "glog/logging.h"
#include "nlohmann/json.hpp"
#include "served/request_error.hpp"
#include "served/served.hpp"
#include "served/status.hpp"
#include "src/detection/engine.h"
#include "src/utils/image_utils.h"
#include "src/utils/label_utils.h"
#include "src/utils/prometheus.h"
#include "src/utils/version.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/model.h"

using nlohmann::json;
using served::multiplexer;
using served::request;
using served::response;
using std::string;
using std::to_string;

namespace restor {

DetectionServer::DetectionServer(
    const string& model_path, const string& label_path, const size_t num_results,
    const string& port, size_t num_threads)
    : m_detection_engine(model_path),
      m_labels(coral::ReadLabelFile(label_path)),
      m_num_results(num_results),
      m_input_tensor_shape(m_detection_engine.get_input_tensor_shape()) {
  LOG(INFO) << "Engine initialized \n";
  LOG(INFO) << "model: " << model_path << "\n";
  LOG(INFO) << "label: " << label_path << "\n";
  LOG(INFO) << "num_results: " << m_num_results << "\n";
  LOG(INFO) << "num_threads: " << num_threads << "\n";
  LOG(INFO) << "input_tensor_shape: " << input_tensor_shape_str();

  m_mux.handle("version").get(
      [this](response& res, const request&) { handle_get_runtime_version(res); });
  m_mux.handle("detects").post(
      [this](response& res, const request& req) { handle_detection(res, req); });
  m_mux.handle("metrics").get([this](response& res, const request&) {
    res << restor::Registry::get_registry().to_string();
  });

  m_server.reset(new served::net::server("0.0.0.0", port, m_mux));
  LOG(INFO) << "Serving on port: " << port << "\n";
  m_server->run(num_threads);
}

void DetectionServer::handle_detection(response& res, const request& req) {
  m_req_id++;
  auto body = json::parse(req.body());
  const auto& data = body["data"].get<string>();

  std::vector<uint8_t> input_tensor = coral::addon::GetInputFromData(
      data, {m_input_tensor_shape[1], m_input_tensor_shape[2], m_input_tensor_shape[3]});

  auto results = m_detection_engine.DetectWithInputTensor(input_tensor);
  auto num_to_return = (results.size() < m_num_results) ? results.size() : m_num_results;
  json j;
  j["req_id"] = m_req_id;

  for (size_t i = 0; i < num_to_return; i++) {
    const auto& result = results.at(i);
    j["result" + to_string(i + 1)] = {{"candidate", m_labels[result.label]},
                                      {"score", result.score}};
    // j["corner"] = result.corners.DebugString();
  }

  LOG(INFO) << j.dump();
  res << j.dump();
}

void DetectionServer::handle_get_runtime_version(response& res) {
  m_req_id++;
  if (m_version_json.empty()) {
    const auto& version = coral::GetRuntimeVersion();
    m_version_json["edgetpu"] = version.substr(version.find_last_of(",") + 2, version.size());
  }
  m_version_json["req_id"] = m_req_id;
  res << m_version_json.dump();
}

string DetectionServer::input_tensor_shape_str() {
  string tensor_str{"["};
  size_t size{m_input_tensor_shape.size() - 1};
  for (const auto i : m_input_tensor_shape) {
    tensor_str.append(to_string(i));
    if (size) tensor_str.append(",");
    size--;
  }
  tensor_str.append("]");
  return tensor_str;
}

}  // namespace restor
