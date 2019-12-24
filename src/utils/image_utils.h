#ifndef EDGETPU_CPP_IMAGE_UTILS_H_
#define EDGETPU_CPP_IMAGE_UTILS_H_

#include <string>
#include <vector>

#include "src/utils/bbox_utils.h"

namespace coral {

// Defines dimension of an image, in height, width, depth order.
typedef std::array<int, 3> ImageDims;

// Gets input from images and resizes to `target_dims`. It will crash upon
// failure.
std::vector<uint8_t> GetInputFromImage(const std::string& image_path, const ImageDims& target_dims);

namespace addon {
// Gets input from data and resizes to `target_dims`
std::vector<uint8_t> GetInputFromData(
    const std::string& base64_bmp_data, const ImageDims& target_dims);
}  // namespace addon
}  // namespace coral

#endif  // EDGETPU_CPP_IMAGE_UTILS_H_
