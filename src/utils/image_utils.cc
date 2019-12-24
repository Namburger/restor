#include "src/utils/image_utils.h"

#include <dirent.h>
#include <sys/types.h>

#include <fstream>
#include <random>
#include <string>

#include "absl/flags/flag.h"
#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "base64/base64.h"
#include "glog/logging.h"
#include "src/basic/basic_engine.h"
#include "src/detection/engine.h"
#include "src/utils/utils.h"
#include "tensorflow/lite/builtin_op_data.h"

namespace coral {

namespace {

using tflite::ops::builtin::BuiltinOpResolver;

// Returns whether string ends with given suffix.
inline bool EndsWith(std::string const& value, std::string const& ending) {
  if (ending.size() > value.size()) return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

// Returns total number of elements.
int ImageDimsToSize(const ImageDims& dims) {
  int size = 1;
  for (const auto& dim : dims) {
    size *= dim;
  }
  return size;
}

std::vector<uint8_t> DecodeBmp(
    const uint8_t* input, int row_size, int width, int height, int channels, bool top_down) {
  std::vector<uint8_t> output(height * width * channels);
  for (int i = 0; i < height; i++) {
    int src_pos;
    int dst_pos;
    for (int j = 0; j < width; j++) {
      if (!top_down) {
        src_pos = ((height - 1 - i) * row_size) + j * channels;
      } else {
        src_pos = i * row_size + j * channels;
      }
      dst_pos = (i * width + j) * channels;
      switch (channels) {
        case 1:
          output[dst_pos] = input[src_pos];
          break;
        case 3:
          // BGR -> RGB
          output[dst_pos] = input[src_pos + 2];
          output[dst_pos + 1] = input[src_pos + 1];
          output[dst_pos + 2] = input[src_pos];
          break;
        case 4:
          // BGRA -> RGBA
          output[dst_pos] = input[src_pos + 2];
          output[dst_pos + 1] = input[src_pos + 1];
          output[dst_pos + 2] = input[src_pos];
          output[dst_pos + 3] = input[src_pos + 3];
          break;
        default:
          LOG(FATAL) << "Unexpected number of channels: " << channels;
          break;
      }
    }
  }
  return output;
}

// Reads BMP image. It will crahs upon failure.
std::vector<uint8_t> ReadBmp(const std::string& input_bmp_name, ImageDims* image_dims) {
  std::string file_content;
  ReadFileOrDie(input_bmp_name, &file_content);
  CHECK(!file_content.empty()) << "Bmp image file is empty " << input_bmp_name;
  const uint8_t* img_bytes = reinterpret_cast<const uint8_t*>(file_content.data());

  // Data in BMP file header is stored in Little Endian format. The following
  // method should work on both Big and Little Endian machine.
  auto to_int32 = [](const unsigned char* p) -> int32_t {
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
  };
  const int32_t header_size = to_int32(img_bytes + 10);
  const int32_t bpp = to_int32(img_bytes + 28);
  int* width = image_dims->data() + 1;
  int* height = image_dims->data();
  int* channels = image_dims->data() + 2;
  *width = to_int32(img_bytes + 18);
  *height = to_int32(img_bytes + 22);
  *channels = bpp / 8;
  // Currently supports RGB and grayscale image at this function.
  CHECK((*width) > 0 && (*height) > 0 && ((*channels) == 3 || (*channels) == 1))
      << "Unsupported image format. width, height, channels: " << *width << ", " << *height << ", "
      << *channels << "\n";

  // there may be padding bytes when the width is not a multiple of 4 bytes
  // 8 * channels == bits per pixel
  const int row_size = (8 * (*channels) * (*width) + 31) / 32 * 4;
  // if height is negative, data layout is top down
  // otherwise, it's bottom up
  bool top_down = (*height < 0);
  // Decode image, allocating tensor once the image size is known
  const uint8_t* bmp_pixels = &img_bytes[header_size];
  return DecodeBmp(bmp_pixels, row_size, *width, abs(*height), *channels, top_down);
}

// Resizes BMP image.
void ResizeImage(
    const ImageDims& in_dims, const uint8_t* in, const ImageDims& out_dims, uint8_t* out) {
  const int image_height = in_dims[0];
  const int image_width = in_dims[1];
  const int image_channels = in_dims[2];
  const int wanted_height = out_dims[0];
  const int wanted_width = out_dims[1];
  const int wanted_channels = out_dims[2];
  const int number_of_pixels = image_height * image_width * image_channels;
  std::unique_ptr<tflite::Interpreter> interpreter(new tflite::Interpreter);
  int base_index = 0;
  // two inputs: input and new_sizes
  interpreter->AddTensors(2, &base_index);
  // one output
  interpreter->AddTensors(1, &base_index);
  // set input and output tensors
  interpreter->SetInputs({0, 1});
  interpreter->SetOutputs({2});
  // set parameters of tensors
  TfLiteQuantizationParams quant;
  interpreter->SetTensorParametersReadWrite(
      0, kTfLiteFloat32, "input", {1, image_height, image_width, image_channels}, quant);
  interpreter->SetTensorParametersReadWrite(1, kTfLiteInt32, "new_size", {2}, quant);
  interpreter->SetTensorParametersReadWrite(
      2, kTfLiteFloat32, "output", {1, wanted_height, wanted_width, wanted_channels}, quant);
  BuiltinOpResolver resolver;
  const TfLiteRegistration* resize_op = resolver.FindOp(tflite::BuiltinOperator_RESIZE_BILINEAR, 1);
  auto* params =
      reinterpret_cast<TfLiteResizeBilinearParams*>(malloc(sizeof(TfLiteResizeBilinearParams)));
  params->align_corners = false;
  interpreter->AddNodeWithParameters({0, 1}, {2}, nullptr, 0, params, resize_op, nullptr);
  interpreter->AllocateTensors();
  // fill input image
  // in[] are integers, cannot do memcpy() directly
  auto input = interpreter->typed_tensor<float>(0);
  for (int i = 0; i < number_of_pixels; i++) {
    input[i] = in[i];
  }
  // fill new_sizes
  interpreter->typed_tensor<int>(1)[0] = wanted_height;
  interpreter->typed_tensor<int>(1)[1] = wanted_width;
  interpreter->Invoke();
  auto output = interpreter->typed_tensor<float>(2);
  auto output_number_of_pixels = wanted_height * wanted_height * wanted_channels;
  for (int i = 0; i < output_number_of_pixels; i++) {
    out[i] = static_cast<uint8_t>(output[i]);
  }
}

// Converts RGB image to grayscale. Take the average.
std::vector<uint8_t> RgbToGrayscale(const std::vector<uint8_t>& in, const ImageDims& in_dims) {
  CHECK_GE(in_dims[2], 3);
  std::vector<uint8_t> result;
  int out_size = in_dims[0] * in_dims[1];
  result.resize(out_size);
  for (size_t in_idx = 0, out_idx = 0; in_idx < in.size(); in_idx += in_dims[2], ++out_idx) {
    int r = in[in_idx];
    int g = in[in_idx + 1];
    int b = in[in_idx + 2];
    result[out_idx] = static_cast<uint8_t>((r + g + b) / 3);
  }
  return result;
}

// Reads BMP data. It will crahs upon failure.
std::vector<uint8_t> ReadBmpData(const std::string& input_bmp_data, ImageDims* image_dims) {
  const uint8_t* img_bytes = reinterpret_cast<const uint8_t*>(input_bmp_data.data());

  // Data in BMP file header is stored in Little Endian format. The following
  // method should work on both Big and Little Endian machine.
  auto to_int32 = [](const unsigned char* p) -> int32_t {
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
  };
  const int32_t header_size = to_int32(img_bytes + 10);
  const int32_t bpp = to_int32(img_bytes + 28);
  int* width = image_dims->data() + 1;
  int* height = image_dims->data();
  int* channels = image_dims->data() + 2;
  *width = to_int32(img_bytes + 18);
  *height = to_int32(img_bytes + 22);
  *channels = bpp / 8;
  // Currently supports RGB and grayscale image at this function.
  CHECK((*width) > 0 && (*height) > 0 && ((*channels) == 3 || (*channels) == 1))
      << "Unsupported image format. width, height, channels: " << *width << ", " << *height << ", "
      << *channels << "\n";

  // there may be padding bytes when the width is not a multiple of 4 bytes
  // 8 * channels == bits per pixel
  const int row_size = (8 * (*channels) * (*width) + 31) / 32 * 4;
  // if height is negative, data layout is top down
  // otherwise, it's bottom up
  bool top_down = (*height < 0);
  // Decode image, allocating tensor once the image size is known
  const uint8_t* bmp_pixels = &img_bytes[header_size];
  return DecodeBmp(bmp_pixels, row_size, *width, abs(*height), *channels, top_down);
}

}  // namespace

std::vector<uint8_t> GetInputFromImage(
    const std::string& image_path, const ImageDims& target_dims) {
  std::vector<uint8_t> result;
  if (!EndsWith(image_path, ".bmp")) {
    LOG(FATAL) << "Unsupported image type: " << image_path;
    return result;
  }
  result.resize(ImageDimsToSize(target_dims));
  ImageDims image_dims;
  std::vector<uint8_t> in = ReadBmp(image_path, &image_dims);
  CHECK(!in.empty()) << "Fail to read bmp image from file: " << image_path;

  if (target_dims[2] == 1 && (image_dims[2] == 3 || image_dims[2] == 4)) {
    in = RgbToGrayscale(in, image_dims);
  }
  ResizeImage(image_dims, in.data(), target_dims, result.data());
  return result;
}

namespace addon {

std::vector<uint8_t> GetInputFromData(
    const std::string& base64_bmp_data, const ImageDims& target_dims) {
  std::vector<uint8_t> result;
  result.resize(ImageDimsToSize(target_dims));
  ImageDims image_dims;
  std::vector<uint8_t> in = ReadBmpData(base64_decode(base64_bmp_data), &image_dims);
  CHECK(!in.empty()) << "Fail to read bmp image from data";

  if (target_dims[2] == 1 && (image_dims[2] == 3 || image_dims[2] == 4)) {
    in = RgbToGrayscale(in, image_dims);
  }
  ResizeImage(image_dims, in.data(), target_dims, result.data());
  return result;
}

}  // namespace addon
}  // namespace coral
