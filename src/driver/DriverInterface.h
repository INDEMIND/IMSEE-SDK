#pragma once
#ifdef WIN32
#define DRIVER_EXPORT extern "C" __declspec(dllexport)
#else
#define DRIVER_EXPORT
#endif
#include <stddef.h>
namespace indem {
/*!
 * 相机返回的数据格式
 */
struct cameraData {
  //时间戳
  double _timeStamp; //单位:毫秒
  //图像数据
  unsigned char *_image;
  //数据宽高，通道数
  int _width, _height, _channel, _size;
};
//相机数据回调函数
typedef void (*DriverCameraDataCallback)(cameraData *data);

/*!
 * 返回的IMU数据格式
 */
struct IMUData {
  //时间戳
  double _timeStamp;
  //[x,y,z]方向的加速度值,单位m/ss
  float _acc[3];
  //[]姿态
  float _gyr[3];
};
/* IMU数据回调函数 */
typedef void (*DriverIMUDataCallback)(IMUData *data);

//图像分辨率
enum IMAGE_RESOLUTION {
  RESOLUTION_640,
  RESOLUTION_1280,
  RESOLUTION_DEFAULT = RESOLUTION_640
};
//接入SDK的模组驱动接口
class IDriverInterface {
public:
  IDriverInterface() {}
  virtual ~IDriverInterface() {}

  //获取模组配置信息,version-结构体的版本号,params-用于接收配置的数组,len-实际拿到数据的长度
  virtual bool GetModuleParams(int &version, unsigned char *params,
                               size_t &len) = 0;
  //设置摄像头回调函数
  virtual void SetCameraCallback(DriverCameraDataCallback callback) = 0;
  //设置IMU回调函数
  virtual void SetIMUCallback(DriverIMUDataCallback callback) = 0;
  //以指定的频率（单位Hz）打开设备
  virtual bool Open(int imuFreq = 1000, int imgFreq = 50,
                    IMAGE_RESOLUTION resolution = RESOLUTION_DEFAULT) = 0;
  //关闭设备
  virtual void Close() = 0;
};
}; // namespace indem

#ifdef __cplusplus
extern "C" {
#endif
/*
 * \brief
 * 在SDK初始化、获取驱动信息的时候该接口都会调用,因此该接口的实现应该尽可能的简单
 */
DRIVER_EXPORT indem::IDriverInterface *DriverFactory();

/* 头显插入拔出回调函数 */
typedef void (*DriverHotplugCallback)(bool bArrive);
DRIVER_EXPORT void SetHotplugCallback(DriverHotplugCallback callback);

#ifdef __cplusplus
}
#endif
