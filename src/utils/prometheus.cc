#include "src/utils/prometheus.h"

#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <regex>
#include <string>
#include <vector>

#include "boost/regex.hpp"
#include "prometheus/gauge.h"
#include "prometheus/registry.h"
#include "prometheus/text_serializer.h"

using std::ifstream;
using std::numeric_limits;
using std::regex;
using std::regex_replace;
using std::stoul;
using std::string;
using std::stringstream;
using std::vector;

namespace restor {

Registry::Registry()
    : m_fds(prometheus::BuildGauge()
                .Name("process_open_fds")
                .Help("Number of open file descriptors")
                .Register(*this)
                .Add({})),
      m_vm_rss(prometheus::BuildGauge()
                   .Name("process_resident_memory_bytes")
                   .Help("Process resident memory size in bytes.")
                   .Register(*this)
                   .Add({})),
      m_vm_size(prometheus::BuildGauge()
                    .Name("process_virtual_memory_bytes")
                    .Help("Process virtual memory size in bytes.")
                    .Register(*this)
                    .Add({})),
      m_vm_peak(prometheus::BuildGauge()
                    .Name("process_virtual_memory_max_bytes")
                    .Help("Process peak virtual memory size in bytes.")
                    .Register(*this)
                    .Add({})) {}

Registry& Registry::get_registry() {
  static Registry r;
  return r;
}

vector<prometheus::MetricFamily> Registry::Collect() const {
  return prometheus::Registry::Collect();
}

void Registry::update_mem_usage() {
  // interested in VmRSS and VmSize
  ifstream proc_status{"/proc/self/status"};
  string line;
  while (proc_status.good()) {
    getline(proc_status, line);
    if (!line.rfind("VmRSS")) {
      m_vm_rss.Set(stoul(regex_replace(line, regex(R"([\D])"), "").c_str()) * 1024);
    } else if (!line.rfind("VmSize")) {
      m_vm_size.Set(stoul(regex_replace(line, regex(R"([\D])"), "").c_str()) * 1024);
    } else if (!line.rfind("VmPeak")) {
      m_vm_peak.Set(stoul(regex_replace(line, regex(R"([\D])"), "").c_str()) * 1024);
    }
  }
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
  update_mem_usage();
}

string Registry::to_string() {
  update_stats();
  return prometheus::TextSerializer().Serialize(Collect());
}

}  // namespace restor
