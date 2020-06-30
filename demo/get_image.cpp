// Copyright 2020 Indemind Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "imrdata.h"
#include "imrsdk.h"
#include "logging.h"
#include "svc_config.h"
#include "times.h"
#include "types.h"
#include <iostream>
#include <queue>
#include <mutex>

#define FONT_FACE cv::FONT_HERSHEY_PLAIN
#define FONT_SCALE 1
#define FONT_COLOR cv::Scalar(255, 255, 255)
#define THICKNESS 1

using namespace indem;

template <typename T> void clear(std::queue<T> &q) {
  std::queue<T> empty;
  swap(empty, q);
}

int main(int argc, char **argv) {
  auto m_pSDK = new CIMRSDK();
  MRCONFIG config = {0};
  config.bSlam = false;
  config.imgResolution = IMG_640;
  config.imgFrequency = 50;
  config.imuFrequency = 1000;

  m_pSDK->Init(config);

  std::queue<cv::Mat> image_queue;
  std::mutex mutex_image;
  int img_count = 0;
  double last_img_time = -1.0;
  m_pSDK->RegistImgCallback([&last_img_time, &img_count, &image_queue,
                                &mutex_image](
                                double time, cv::Mat left, cv::Mat right) {
    if (!left.empty() && !right.empty()) {
      cv::Mat img;
      cv::hconcat(left, right, img);
      if (last_img_time >= 0) {
        std::ostringstream ss;
        double fps = 1.0 / (time - last_img_time);
        ss << std::fixed << "time stamp: " << std::setprecision(2) << time
           << ", fps: " << fps;
        cv::putText(img, ss.str(), cv::Point(25, 25), FONT_FACE, FONT_SCALE,
                    FONT_COLOR, THICKNESS);
      }
      last_img_time = time;
      {
        std::unique_lock<std::mutex> lock(mutex_image);
        image_queue.push(img);
      }
      ++img_count;
    }
  });
  auto &&time_beg = times::now();
  while (true) {
    if (!image_queue.empty()) {
      std::unique_lock<std::mutex> lock(mutex_image);
      cv::imshow("image", image_queue.front());
      clear(image_queue);
    }
    char key = static_cast<char>(cv::waitKey(1));
    if (key == 27 || key == 'q' || key == 'Q') { // ESC/Q
      break;
    }
  }
  delete m_pSDK;

  auto &&time_end = times::now();

  float elapsed_ms =
      times::count<times::microseconds>(time_end - time_beg) * 0.001f;
#ifdef __linux
  LOG(INFO) << "Time beg: " << times::to_local_string(time_beg)
            << ", end: " << times::to_local_string(time_end)
            << ", cost: " << elapsed_ms << "ms";
  LOG(INFO) << "Img count: " << img_count
            << ", fps: " << (1000.f * img_count / elapsed_ms);
#endif
  return 0;
}
