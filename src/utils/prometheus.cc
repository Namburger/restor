#include "src/utils/prometheus.h"

#include <sys/stat.h>

#include <limits>
#include <string>
#include <vector>

#include "prometheus/gauge.h"
#include "prometheus/registry.h"
#include "prometheus/text_serializer.h"

using std::numeric_limits;
using std::string;
using std::vector;

namespace restor {

Registry::Registry()
    : m_fds(prometheus::BuildGauge()
                .Name("open_fds")
                .Help("Number of open file descriptors")
                .Register(*this)
                .Add({})) {}

Registry& Registry::get_registry() {
  static Registry r;
  return r;
}

vector<prometheus::MetricFamily> Registry::Collect() const {
  return prometheus::Registry::Collect();
}

void Registry::update_stats() {
  int fd_counter{0};
  struct stat stats;
  for (int i = 0; i < 1024; i++) {
    fstat(i, &stats);
    if (errno != EBADF) {
      fd_counter++;
    }
  }
  m_fds.Set(fd_counter);
}

string Registry::to_string() {
  update_stats();
  return prometheus::TextSerializer().Serialize(Collect());
}

}  // namespace restor
