#include "src/utils/server.h"

#include "glog/logging.h"
#include "nlohmann/json.hpp"
#include "prometheus/counter.h"
#include "prometheus/registry.h"
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
    const string& port, const size_t num_threads)
    : m_detection_engine(model_path),
      m_labels(coral::ReadLabelFile(label_path)),
      m_num_results(num_results),
      m_input_tensor_shape(m_detection_engine.get_input_tensor_shape()),
      m_requests_counter(prometheus::BuildCounter()
                             .Name("server_request_total")
                             .Help("Number of total http requests handled")
                             .Register(restor::Registry::get_registry())) {
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
    m_requests_counter.Add({{"method", "GET"}, {"endpoint", "metrics"}}).Increment();
    res.set_header("Server", "Restor");
    res << restor::Registry::get_registry().to_string();
  });
  m_mux.handle("info").get([&, this](response& res, const request&) {
    handle_get_server_info(res, model_path, label_path, num_threads);
  });

  LOG(INFO) << "End points registered";
  m_server.reset(new served::net::server("0.0.0.0", port, m_mux));
  LOG(INFO) << "Serving on port: " << port << "\n";
  m_server->run(num_threads);
}

void DetectionServer::handle_get_server_info(
    response& res, const string& model, const std::string& label, const size_t num_threads) {
  m_requests_counter.Add({{"method", "GET"}, {"endpoint", "info"}}).Increment();
  m_req_id++;
  res.set_header("Server", "Restor");
  json j;
  j["model_path"] = model;
  j["input_tensor_shape"] = input_tensor_shape_str();
  j["label_path"] = label;
  j["num_threads"] = num_threads;
  j["num_results"] = m_num_results;
  j["end_points"] = "GET/metrics, GET/info, GET/version, POST/detects";
  LOG(INFO) << j.dump();
  res << j.dump();
}

void DetectionServer::handle_detection(response& res, const request& req) {
  m_requests_counter.Add({{"method", "POST"}, {"endpoint", "detects"}}).Increment();
  m_req_id++;
  res.set_header("Server", "Restor");
  auto body = json::parse(req.body());
  const auto& data = body["data"].get<string>();

  std::vector<uint8_t> input_tensor = coral::addon::GetInputFromData(
      data, {m_input_tensor_shape[1], m_input_tensor_shape[2], m_input_tensor_shape[3]});
  if (input_tensor.empty()) {
    res.set_status(served::status_4XX::NOT_ACCEPTABLE);
    json j;
    j["req_id"] = m_req_id;
    j["error"] = "Unable to process data";
    j["message"] = "Please make sure data are uncorrupted base64 bmp format";
    LOG(INFO) << j.dump();
    res << j.dump();
    return;
  }

  auto results = m_detection_engine.DetectWithInputTensor(input_tensor);
  auto num_to_return = (results.size() < m_num_results) ? results.size() : m_num_results;

  json j;
  j["req_id"] = m_req_id;
  if (!num_to_return) j["result"] = "None";

  for (auto i = 0; i < num_to_return; i++) {
    const auto& result = results.at(i);
    j["result" + to_string(i + 1)] = {
        {"candidate", m_labels[result.label]}, {"score", result.score},
        {"xmin", result.corners.xmin},         {"xmax", result.corners.xmax},
        {"ymin", result.corners.ymin},         {"ymax", result.corners.ymax},
    };
  }

  LOG(INFO) << j.dump();
  res << j.dump();
}

void DetectionServer::handle_get_runtime_version(response& res) {
  m_requests_counter.Add({{"method", "GET"}, {"endpoint", "version"}}).Increment();
  m_req_id++;
  res.set_header("Server", "Restor");
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
