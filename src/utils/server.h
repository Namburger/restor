#include "nlohmann/json.hpp"
#include "served/request_error.hpp"
#include "served/served.hpp"
#include "served/status.hpp"
#include "src/detection/engine.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/model.h"

namespace restor {

class DetectionServer {
public:
  DetectionServer(
      const std::string& modeil_path, const std::string& label_path, const size_t num_results,
      const std::string& port, const size_t num_threads);

  void handle_detection(served::response& res, const served::request& req);
  void handle_get_runtime_version(served::response& res);
  std::string input_tensor_shape_str();

private:
  coral::DetectionEngine m_detection_engine;
  std::unordered_map<int, std::string> m_labels;
  size_t m_num_results;
  std::vector<int> m_input_tensor_shape;

  served::multiplexer m_mux;
  std::shared_ptr<served::net::server> m_server;
  size_t m_req_id{0};
  nlohmann::json m_version_json;
};

}  // namespace restor
