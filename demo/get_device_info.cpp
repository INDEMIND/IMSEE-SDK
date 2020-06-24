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

using namespace indem;

int main(int argc, char **argv) {
  auto m_pSDK = new CIMRSDK();
  MRCONFIG config = {0};
  config.bSlam = false;
  config.imgResolution = IMG_640;
  config.imgFrequency = 50;
  config.imuFrequency = 1000;

  // m_pSDK->Init(config);
  indem::MoudleAllParam param = m_pSDK->GetModuleParams();
  indem::ModuleInfo info = m_pSDK->GetModuleInfo();

  std::cout << "Module info: " << std::endl;
  info.printInfo();
  std::cout << "Left param: " << std::endl;
  param._left_camera[RESOLUTION::RES_640X400].printInfo();
  std::cout << "Right param: " << std::endl;
  param._right_camera[RESOLUTION::RES_640X400].printInfo();
  std::cout << "Imu param: " << std::endl;
  param._imu.printInfo();
  delete m_pSDK;
  return 0;
}
