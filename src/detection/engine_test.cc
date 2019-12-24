#include "src/detection/engine.h"

#include "absl/flags/parse.h"
#include "glog/logging.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "src/utils/test_utils.h"

namespace coral {
namespace {

using ::testing::ElementsAre;

TEST(DetectionEngineTest, TestDebugFunctions) {
  // Load the model.
  DetectionEngine engine(TestDataPath("mobilenet_ssd_v1_coco_quant_postprocess.tflite"));
  // Check input dimensions.
  std::vector<int> input_tensor_shape = engine.get_input_tensor_shape();
  ASSERT_EQ(4, input_tensor_shape.size());
  EXPECT_THAT(input_tensor_shape, ElementsAre(1, 300, 300, 3));
  // Check output tensors.
  std::vector<int> output_tensor_sizes = engine.get_all_output_tensors_sizes();
  ASSERT_EQ(4, output_tensor_sizes.size());
  EXPECT_THAT(output_tensor_sizes, ElementsAre(80, 20, 20, 1));
  // Check model's path.
  EXPECT_EQ(TestDataPath("mobilenet_ssd_v1_coco_quant_postprocess.tflite"), engine.model_path());
}

}  // namespace
}  // namespace coral

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  absl::ParseCommandLine(argc, argv);
  return RUN_ALL_TESTS();
}
