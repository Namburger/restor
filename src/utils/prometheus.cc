#include "src/utils/prometheus.h"

#include <sys/stat.h>
#include <unistd.h>

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
                .Name("process_open_fds")
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
  // setting file desscriptors
  auto max_fds = getdtablesize();
  int fd_counter{0};
  struct stat stats;
  for (int i = 0; i < max_fds; i++) {
    if (fstat(i, &stats); errno != EBADF) {
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
