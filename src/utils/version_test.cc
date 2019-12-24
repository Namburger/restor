#include "src/utils/version.h"

#include "gtest/gtest.h"

namespace coral {
TEST(VersionTest, CheckDriverVersion) {
  EXPECT_NE(GetRuntimeVersion().find(kSupportedRuntimeVersion), std::string::npos);
}
}  // namespace coral
