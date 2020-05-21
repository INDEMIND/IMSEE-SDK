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
#ifndef TYPES_H_
#define TYPES_H_
#include <functional>
#include <iostream>
#include <map>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <type_traits>
#include <vector>
#pragma once
namespace indem {
#define SLAM_LOOSE_ID 1
const static int32_t DATA_LEFT = 0x0001;
const static int32_t DATA_RIGHT = 0x0002;

const static int32_t MSG_IMU = 0x0004;
const static int32_t MSG_IMAGE = 0x0008;

const static int32_t SRC_HMD = 0x000100;
const static int32_t SRC_USB = 0x002000;

enum RESOLUTION { RES_640X400, RES_1280X800, RES_DEFAULT = RES_640X400 };

enum DepthCalMode { HIGH_SPEED, HIGH_ACCURACY };

typedef struct {
  double time;
  float acc[3];
  float gyr[3];
  bool COME_FLAG;
  bool flag;
} IMU_HANDLE;

typedef struct {
  double time;
  float acc[3];
  float gyr[3];
  bool COME_FLAG;
} IMU_HEADSET;

typedef struct {
  double time;
  float p[3];
  float q[4];
  float vel[3];
  bool COME_FLAG;
  bool flag;
} SLAM_LOOSE_OUT;

typedef struct {
  double time;
  float p[3];
  float q[4];
  float vel[3];
  float rms[9];
  bool COME_FLAG;
  bool flag;
  bool LI_flag;
} SLAM_PURE_OUT;

typedef struct {
  SLAM_PURE_OUT salm_pure_out;
  SLAM_LOOSE_OUT slam_loose_out;
} HEADSET_OUT;

typedef struct {
  float time;
  float p[3];
  float q[4];
  bool COME_FLAG;
  bool flag;
  int pattern;
} HANDLE_LOOSE_OUT;

typedef struct {
  float time; // s
  float p[3]; // m
  float q[4];
  bool COME_FLAG = false;
  bool flag = false;
  int mPointNum = -1;
  double mAveError = 1.7976931348623158e+308;
  int pattern;
} HANDLE_PURE_OUT;

typedef struct {
  float time;
  float p[3];
  float q[4];
  bool COME_FLAG;
  bool flag;
} HANDLE_LOOSE_PREDICT;

/*
struct HANDLE_IN {
  cv::Mat pImg1;
  cv::Mat pImg2;
  double time;
  HANDLE_LOOSE_PREDICT predict[2];
  int pattern[2];
};

typedef struct {
  HANDLE_PURE_OUT handle_out[2];
} HANDLE_OUT;
*/

struct ImuData {
  double timestamp;
  /** IMU accelerometer data for 3-axis: X, Y, Z. */
  float accel[3];
  /** IMU gyroscope data for 3-axis: X, Y, Z. */
  float gyro[3];
  ImuData() {}
  ImuData(double time, float accel_x, float accel_y, float accel_z,
          float gyro_x, float gyro_y, float gyro_z) {
    timestamp = time;
    accel[0] = accel_x;
    accel[1] = accel_y;
    accel[2] = accel_z;
    gyro[0] = gyro_x;
    gyro[1] = gyro_y;
    gyro[2] = gyro_z;
  }
};

struct CameraParameter {
  double _TSC[16]; // 4X4 camera to imu
  int _width;
  int _height;

  // distortion_type:equidistant
  double _focal_length[2];    // fx,fy
  double _principal_point[2]; // cx,cy
  // Rectification matrix (stereo cameras only)
  // A rotation matrix aligning the camera coordinate system to the ideal
  // stereo image plane so that epipolar lines in both stereo images are
  // parallel.
  double _R[9];
  // Projection/camera matrix
  //     [fx'  0  cx' Tx]
  // P = [ 0  fy' cy' Ty]
  //     [ 0   0   1   0]
  double _P[12];
  // Intrinsic camera matrix for the raw (distorted) images.
  //     [fx  0 cx]
  // K = [ 0 fy cy]
  //     [ 0  0  1]
  double _K[9];
  // The distortion parameters, size depending on the distortion model.
  // For us, the 4 parameters are: (k1, k2, t1, t2).
  double _D[4];

  CameraParameter resize(double ratio) const {
    CameraParameter parameter;
    parameter = *this;
    parameter._width = static_cast<int>(this->_width * ratio);
    parameter._height = static_cast<int>(this->_height * ratio);
    for (int m = 0; m < 2; m++) {
      parameter._focal_length[m] = ratio * this->_focal_length[m];
    }
    for (int n = 0; n < 2; n++) {
      parameter._principal_point[n] = ratio * this->_principal_point[n];
    }
    for (int i = 0; i < 12; i++) {
      if (i != 10)
        parameter._P[i] = ratio * this->_P[i];
    }
    for (int j = 0; j < 9; j++) {
      if (j != 8)
        parameter._K[j] = ratio * this->_K[j];
    }
    return parameter;
  }
  void printInfo() {
    std::cout << "_width: " << _width << ", height: " << _height << std::endl;
    for (int i = 0; i < 2; i++) {
      std::cout << "_focal_length[" << i << "]: " << _focal_length[i]
                << std::endl;
      std::cout << "_principal_point[" << i << "]: " << _principal_point[i]
                << std::endl;
    }
    for (int i = 0; i < 16; i++) {
      std::cout << "_TSC[" << i << "]: " << _TSC[i] << std::endl;
    }
    for (int i = 0; i < 9; i++) {
      std::cout << "_R[" << i << "]: " << _R[i] << std::endl;
      std::cout << "_K[" << i << "]: " << _K[i] << std::endl;
    }
    for (int i = 0; i < 12; i++) {
      std::cout << "_P[" << i << "]: " << _P[i] << std::endl;
    }
  }
};
// IMU 参数
struct IMUParameter {
  double _a_max;
  double _g_max;
  double _sigma_g_c;
  double _sigma_a_c;
  double _sigma_bg;
  double _sigma_ba;
  double _sigma_gw_c;
  double _sigma_aw_c;
  double _tau;
  double _g;
  double _a0[4];
  double _T_BS[16];
  double _Acc[12]; //加速度计的补偿参数
  double _Gyr[12]; //陀螺仪的补偿参数
};

struct ModuleInfo {
  char _id[32];
  char _designer[32];
  char _fireware_version[32];
  char _hardware_version[32];
  char _lens[32];
  char _imu[32];
  char _viewing_angle[32];
  char _baseline[32];
};

struct SlamParameter {
  int _numKeyframes;            //关键帧数量 =5
  int _numImuFrames;            //相邻imu帧数量 =3
  int _ceres_minIterations;     //最小优化次数 =3
  int _ceres_maxIterations;     //最大优化次数 =4
  double _ceres_timeLimit;      //=3.5000000000000003e-02
  int detection_threshold;      //角点阈值 =30
  int detection_octaves;        //=0
  int detection_maxNoKeypoints; //最大角点数量    = 100
  bool displayImages;           //是否显示slam界面  = true
};

// TODO: Replace 'ModuleParameters' at all
struct MoudleAllParam {
  std::map<RESOLUTION, CameraParameter> _left_camera;
  std::map<RESOLUTION, CameraParameter> _right_camera;
  int _camera_channel = 1; // TODO: Put this parameter in 'CameraParameter'
  double _baseline = 0.12; // TODO: Put this parameter in 'CameraParameter'
  IMUParameter _imu;
  ModuleInfo _device;
  SlamParameter _slam;
};

struct ModuleParameters {
  CameraParameter _camera[2]; //左右目相机
  IMUParameter _imu;
  ModuleInfo _device;
  SlamParameter _slam;
};

// detector
enum CLASS_NAME {
  BG = 0,
  PERSON,
  PET_CAT,
  PET_DOG,
  SOFA,
  TABLE,
  BED,
  EXCREMENT,
  WIRE,
  KEY,
};

struct BoxInfo {
  cv::Rect box;
  float score;
  float cx;
  float cy;
  // in some case, float type is needed
  float width;
  float height;

  // class
  CLASS_NAME class_name;

  // used for record its position
  int index;
};

struct InstanceInfo {
  // class
  CLASS_NAME class_name;

  // instance
  int instance_id;

  // xyz
  float location[3];
  // camera coords
  float location_cam[3];

  // time
  float time;

  // scale
  float scale;

  // used calc iou in image
  cv::Rect box;

  int _count;
  float front_face_points[3][3];
  float rear_face_points[3][3];
  bool visible;

  int class_count = 0;

  bool valid;

  // calc from location instead of center of box
  float cx;
  float cy;
  float depth;
};

struct DetectorInfo {
  double timestamp;
  cv::Mat img;
  std::vector<BoxInfo> finalBoxInfo;
};

#define MODULE_SIZE sizeof(ModuleParameters)

template <int VERSION_NUM> struct ModuleParamInFlash {};

//版本0对应的数据结构，旧版本
template <> struct ModuleParamInFlash<0> { ModuleParameters _parent; };
//版本1对应的数据结构
template <> struct ModuleParamInFlash<1> {
  ModuleParameters _parent;
  int _imgFrequency;  //图像频率
  int _imgResolution; //图像分辨率
  int _imuFrequency;  // IMU频率
};
#define FLASH_PARAMETER_SIZE_1 sizeof(ModuleParamInFlash<1>)
//模组类型
enum ModuleType {
  NORMAL_MODULE, //默认值,模组
  AR_MODULE,     // AR
  MR_MODULE,     // AR
};
//版本2对应的数据结构
template <> struct ModuleParamInFlash<2> {
  ModuleParamInFlash<1> _parent;
  ModuleType _type;
};
#define FLASH_PARAMETER_SIZE_2 sizeof(ModuleParamInFlash<2>)

///////////////////////////////静态断言POD保证新版数据类型能够使用memcpy////////////////////////////////////
static_assert(std::is_pod<ModuleParamInFlash<1>>::value == true,
              "ModuleParamInFlash<1> is not POD");
static_assert(std::is_pod<ModuleParamInFlash<2>>::value == true,
              "ModuleParamInFlash<2> is not POD");
static_assert(std::is_pod<ModuleParamInFlash<3>>::value == true,
              "ModuleParamInFlash<3> is not POD");
static_assert(std::is_pod<ModuleParamInFlash<4>>::value == true,
              "ModuleParamInFlash<4> is not POD");
static_assert(std::is_pod<ModuleParamInFlash<5>>::value == true,
              "ModuleParamInFlash<5> is not POD");

#define FLASH_MAX_SIZE FLASH_PARAMETER_SIZE_2

} // namespace indem
#endif // TYPES_H_
