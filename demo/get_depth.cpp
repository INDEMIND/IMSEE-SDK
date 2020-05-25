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
  std::queue<cv::Mat> depth_queue;
  int depth_count = 0;
  if (m_pSDK->EnableDepthProcessor()) {
    m_pSDK->RegistDepthCallback(
        [&depth_count, &depth_queue](double time, cv::Mat depth) {
          if (!depth.empty()) {
            depth.convertTo(depth, CV_16U, 1000.0);
            depth_queue.push(depth);
            ++depth_count;
          }
        });
  }
  auto &&time_beg = times::now();
  while (true) {
    if (!depth_queue.empty()) {
      cv::imshow("depth", depth_queue.front());
      depth_queue.pop();
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
  LOG(INFO) << "depth count: " << depth_count
            << ", fps: " << (1000.f * depth_count / elapsed_ms);
#endif
  return 0;
}