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
#include "times.h"
#include "types.h"
#include <queue>

using namespace indem;

int main(int argc, char **argv) {
  CIMRSDK *m_pSDK = new CIMRSDK();
  MRCONFIG config = {0};
  config.bSlam = false;
  config.imgResolution = IMG_640;
  config.imgFrequency = 50;
  config.imuFrequency = 1000;

  m_pSDK->Init(config);

  std::queue<cv::Mat> image_queue;

  int img_count = 0;
  m_pSDK->RegistImgCallback(
      [&img_count, &image_queue](double time, cv::Mat left, cv::Mat right) {
        if (!left.empty() && !right.empty()) {
          cv::Mat img;
          cv::hconcat(left, right, img);
          image_queue.push(img);
          ++img_count;
        }
      });
  int imu_count = 0;
  m_pSDK->RegistModuleIMUCallback([&imu_count](ImuData imu) {
    if (imu_count % 100 == 0) {
#ifdef __linux
      LOG(INFO) << "imu timestamp: " << imu.timestamp
                << ", accel x: " << imu.accel[0]
                << ", accel y: " << imu.accel[1]
                << ", accel z: " << imu.accel[2] << ", gyro x: " << imu.gyro[0]
                << ", gyro y: " << imu.gyro[1] << ", gyro z: " << imu.gyro[2];
#endif
    }
    ++imu_count;
  });
  auto &&time_beg = times::now();
  while (true) {
    if (!image_queue.empty()) {
      cv::imshow("image", image_queue.front());
      image_queue.pop();
    }
    char key = static_cast<char>(cv::waitKey(1));
    if (key == 27 || key == 'q' || key == 'Q') { // ESC/Q
      break;
    }
  }
  auto &&time_end = times::now();

  float elapsed_ms =
      times::count<times::microseconds>(time_end - time_beg) * 0.001f;
#ifdef __linux
  LOG(INFO) << "Time beg: " << times::to_local_string(time_beg)
            << ", end: " << times::to_local_string(time_end)
            << ", cost: " << elapsed_ms << "ms";
  LOG(INFO) << "Img count: " << img_count
            << ", fps: " << (1000.f * img_count / elapsed_ms);
  LOG(INFO) << "Imu count: " << imu_count
            << ", hz: " << (1000.f * imu_count / elapsed_ms);
#endif
  delete m_pSDK;
  return 0;
}