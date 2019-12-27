#include <string>
#include <vector>

#include "prometheus/registry.h"

namespace restor {

class Registry : public prometheus::Registry {
public:
  Registry();
  static Registry& get_registry();
  std::string to_string();
  std::vector<prometheus::MetricFamily> Collect() const override;
  void update_stats();
  void update_mem_usage();

private:
  prometheus::Gauge& m_fds;
  prometheus::Gauge& m_vm_rss;
  prometheus::Gauge& m_vm_size;
  prometheus::Gauge& m_vm_peak;
};

}  // namespace restor
