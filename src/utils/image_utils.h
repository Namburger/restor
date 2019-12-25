/* Copyright 2019 Google LLC
 *
 * Modifications copyright(C) 2019 by Nam Vu
 *
 * Licensed under the Apache License, Version 2.0(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https:  // www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
