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
#include <mutex>
#include <queue>
#include <thread>
#ifdef WIN32
#include <Windows.h>
#define PATH_MAX 4096
#endif
using namespace indem;

std::string GetRoot() {
  char szPath[PATH_MAX] = {0};
#ifdef WIN32
  GetModuleFileName(NULL, szPath, PATH_MAX);
#else
  int i;
  int rslt = readlink("/proc/self/exe", szPath, PATH_MAX - 1);
  if (rslt < 0 || (rslt >= PATH_MAX - 1)) {
    return NULL;
  }
#endif
  std::string strPath(szPath);
#ifdef WIN32
  strPath = strPath.substr(0, strPath.find_last_of('\\'));
#else
  strPath = strPath.substr(0, strPath.find_last_of('/'));
#endif
  return std::move(strPath);
}

int main(int argc, char **argv) {
  CIMRSDK *m_pSDK = new CIMRSDK();
  MRCONFIG config = {0};
  config.bSlam = false;
  config.imgResolution = IMG_640;
  config.imgFrequency = 50;
  config.imuFrequency = 1000;

  m_pSDK->Init(config);

  std::queue<cv::Mat> image_queue;
  std::queue<ImuData> imu_queue;

  std::mutex mtx;

  int img_count = 0;
  bool is_stop = false;

  std::thread record_thread([&]() {
    std::ofstream ofs;
    ImuData imu;
#ifdef WIN32
    ofs.open(GetRoot() + "\\imudata.txt", std::ofstream::out);
#else
    ofs.open(GetRoot() + "/imudata.txt", std::ofstream::out);
#endif

    ofs << "date: " << times::to_local_string(times::now()) << std::endl;
    ofs << "timestamp, accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z"
        << std::endl;
    while (!is_stop || !imu_queue.empty()) {
      if (!imu_queue.empty()) {
        imu = imu_queue.front();
        ofs << imu.timestamp << ", " << imu.accel[0] << ", " << imu.accel[1]
            << ", " << imu.accel[2] << ", " << imu.gyro[0] << ", "
            << imu.gyro[1] << ", " << imu.gyro[2] << std::endl;
        imu_queue.pop();
      } else {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
      }
    }
#ifdef __linux
    LOG(INFO) << "Finish save imu data!";
#endif
    ofs.flush();
    ofs.close();
  });

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
  m_pSDK->RegistModuleIMUCallback([&imu_count, &imu_queue](ImuData imu) {
    imu_queue.push(imu);
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
      is_stop = true;
      break;
    }
  }
  delete m_pSDK;
  auto &&time_end = times::now();

  record_thread.join();
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
  return 0;
}