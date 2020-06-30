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
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <queue>
#include <mutex>
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
  std::queue<cv::Mat> disparity_queue;
  std::mutex mutex_disparity;
  int disparity_count = 0;
  if (m_pSDK->EnableDisparityProcessor()) {
    // m_pSDK->EnableLRConsistencyCheck();
    // m_pSDK->SetDepthCalMode(DepthCalMode::HIGH_ACCURACY);
    m_pSDK->RegistDisparityCallback(
        [&disparity_count, &disparity_queue, &mutex_disparity](double time, cv::Mat disparity) {
          if (!disparity.empty()) {
            disparity.convertTo(disparity, CV_8U, 255. / (16 * 64));
            cv::applyColorMap(disparity, disparity, cv::COLORMAP_JET);
            disparity.setTo(0, disparity == -16);
            {
                std::unique_lock<std::mutex> lock(mutex_disparity);
                disparity_queue.push(disparity);
            }
            ++disparity_count;
          }
        });
  }
  auto &&time_beg = times::now();
  while (true) {
    if (!disparity_queue.empty()) {
      std::unique_lock<std::mutex> lock(mutex_disparity);
      cv::imshow("disparity", disparity_queue.front());
      clear(disparity_queue);
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
  LOG(INFO) << "disparity count: " << disparity_count
            << ", fps: " << (1000.f * disparity_count / elapsed_ms);
#endif
  return 0;
}